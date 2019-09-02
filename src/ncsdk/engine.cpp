/*  Copyright (C) 2014-2019 FastoGT. All right reserved.

    This file is part of FastoML.

    FastoML is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    FastoML is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with FastoML. If not, see <http://www.gnu.org/licenses/>.
*/

#include <fastoml/ncsdk/engine.h>

#include <common/string_util.h>

#include <fastoml/ncsdk/frame.h>
#include <fastoml/ncsdk/model.h>
#include <fastoml/ncsdk/parameters.h>
#include <fastoml/ncsdk/prediction.h>
#include <fastoml/ncsdk/types.h>

namespace fastoml {
namespace ncsdk {

Engine::Engine()
    : movidius_device_(nullptr),
      input_buffers_(nullptr),
      output_buffers_(nullptr) {}

const BackendMeta Engine::meta = {NCSDK, "NCSDK", "Intel Movidius Neural Compute software developer kit", "2"};

BackendMeta Engine::GetBackendMeta() const {
  return meta;
}

common::ErrnoError Engine::SetProperty(const std::string& property, common::Value* value) {
  if (property.empty() || !value) {
    return common::make_errno_error_inval();
  }

  for (size_t i = 0; i < kParameters.size(); ++i) {
    const ParameterMeta prop = kParameters[i];
    if (common::EqualsASCII(prop.name, property, false)) {
      if (prop.type == value->GetType()) {
        if (prop.name == VERSION_PROPERTY) {
          return common::make_errno_error("Property is not writeable", EINVAL);
        }
      }

      return common::make_errno_error("Invalid value type", EINVAL);
    }
  }

  return common::make_errno_error("Propery not found", EINVAL);
}

common::ErrnoError Engine::GetProperty(const std::string& property, common::Value** value) {
  if (property.empty() || !value) {
    return common::make_errno_error_inval();
  }

  for (size_t i = 0; i < kParameters.size(); ++i) {
    const ParameterMeta prop = kParameters[i];
    if (common::EqualsASCII(prop.name, property, false)) {
      if (prop.name == VERSION_PROPERTY) {
        *value = common::Value::CreateStringValueFromBasicString(meta.version);
        return common::ErrnoError();
      }
    }
  }

  return common::make_errno_error("Propery not found", EINVAL);
}

common::ErrnoError Engine::MakeFrame(const common::draw::Size& size,
                                     ImageFormat::Type format,
                                     void* data,
                                     IFrame** frame) {
  if (!frame) {
    return common::make_errno_error_inval();
  }

  *frame = new Frame(size, format, data);
  return common::ErrnoError();
}

common::ErrnoError Engine::StartImpl() {
  common::ErrnoError err = model_->Start();
  if (err) {
    return err;
  }

  ncDeviceHandle_t* device_handle = nullptr;
  ncStatus_t ret = ncDeviceCreate(0, &device_handle);
  if (ret != NC_OK) {
    return common::make_errno_error(GetStringFromStatus(ret), EINVAL);
  }

  ret = ncDeviceOpen(device_handle);
  if (ret != NC_OK) {
    ncDeviceDestroy(&device_handle);
    return common::make_errno_error(GetStringFromStatus(ret), EINVAL);
  }

  Model* model = static_cast<Model*>(model_);
  size_t model_size = model->GetDataSize();
  void* model_ptr = model->GetData();
  ncGraphHandle_t* model_handle = model->GetHandler();

  ret = ncGraphAllocate(device_handle, model_handle, model_ptr, model_size);
  if (ret != NC_OK) {
    ncDeviceClose(device_handle);
    ncDeviceDestroy(&device_handle);
    return common::make_errno_error(GetStringFromStatus(ret), EINVAL);
  }

  unsigned int descriptor_length = sizeof(struct ncTensorDescriptor_t);
  ncTensorDescriptor_t linput_descriptor;
  ret = ncGraphGetOption(model_handle, NC_RO_GRAPH_INPUT_TENSOR_DESCRIPTORS, &linput_descriptor, &descriptor_length);
  if (ret != NC_OK) {
    ncGraphDestroy(&model_handle);
    ncDeviceClose(device_handle);
    ncDeviceDestroy(&device_handle);
    return common::make_errno_error(GetStringFromStatus(ret), EINVAL);
  }

  ncTensorDescriptor_t loutput_descriptor;
  ret = ncGraphGetOption(model_handle, NC_RO_GRAPH_OUTPUT_TENSOR_DESCRIPTORS, &loutput_descriptor, &descriptor_length);
  if (ret != NC_OK) {
    ncGraphDestroy(&model_handle);
    ncDeviceClose(device_handle);
    ncDeviceDestroy(&device_handle);
    return common::make_errno_error(GetStringFromStatus(ret), EINVAL);
  }

  ncFifoHandle_t* input_buffers_ptr = nullptr;
  ret = ncFifoCreate("FifoIn0", NC_FIFO_HOST_WO, &input_buffers_ptr);
  if (ret != NC_OK) {
    ncGraphDestroy(&model_handle);
    ncDeviceClose(device_handle);
    ncDeviceDestroy(&device_handle);
    return common::make_errno_error(GetStringFromStatus(ret), EINVAL);
  }

  ret = ncFifoAllocate(input_buffers_ptr, device_handle, &linput_descriptor, 2);
  if (ret != NC_OK) {
    ncFifoDestroy(&input_buffers_ptr);
    ncGraphDestroy(&model_handle);
    ncDeviceClose(device_handle);
    ncDeviceDestroy(&device_handle);
    return common::make_errno_error(GetStringFromStatus(ret), EINVAL);
  }

  ncFifoHandle_t* output_buffers_ptr = nullptr;
  ret = ncFifoCreate("FifoOut0", NC_FIFO_HOST_RO, &output_buffers_ptr);
  if (ret != NC_OK) {
    ncFifoDestroy(&input_buffers_ptr);
    ncGraphDestroy(&model_handle);
    ncDeviceClose(device_handle);
    ncDeviceDestroy(&device_handle);
    return common::make_errno_error(GetStringFromStatus(ret), EINVAL);
  }

  ret = ncFifoAllocate(output_buffers_ptr, device_handle, &loutput_descriptor, 2);
  if (ret != NC_OK) {
    ncFifoDestroy(&output_buffers_ptr);
    ncFifoDestroy(&input_buffers_ptr);
    ncGraphDestroy(&model_handle);
    ncDeviceClose(device_handle);
    ncDeviceDestroy(&device_handle);
    return common::make_errno_error(GetStringFromStatus(ret), EINVAL);
  }

  input_buffers_ = input_buffers_ptr;
  output_buffers_ = output_buffers_ptr;
  movidius_device_ = device_handle;
  return common::ErrnoError();
}

common::ErrnoError Engine::StopImpl() {
  ncStatus_t ret = NC_OK;
  if (output_buffers_) {
    ret = ncFifoDestroy(&output_buffers_);
    if (ret != NC_OK) {
      return common::make_errno_error(GetStringFromStatus(ret), EINVAL);
    }
    output_buffers_ = nullptr;
  }

  if (input_buffers_) {
    ret = ncFifoDestroy(&input_buffers_);
    if (ret != NC_OK) {
      return common::make_errno_error(GetStringFromStatus(ret), EINVAL);
    }
    input_buffers_ = nullptr;
  }

  common::ErrnoError err = model_->Stop();
  if (err) {
    return err;
  }

  if (movidius_device_) {
    ret = ncDeviceClose(movidius_device_);
    if (ret != NC_OK) {
      return common::make_errno_error(GetStringFromStatus(ret), EINVAL);
    }

    ret = ncDeviceDestroy(&movidius_device_);
    if (ret != NC_OK) {
      return common::make_errno_error(GetStringFromStatus(ret), EINVAL);
    }
    movidius_device_ = nullptr;
  }

  return common::ErrnoError();
}

common::ErrnoError Engine::PredictImpl(IFrame* in_frame, IPrediction** pred) {
  Model* model = static_cast<Model*>(model_);

  float* data = static_cast<float*>(in_frame->GetData());
  ImageFormat in_format = in_frame->GetFormat();
  common::draw::Size size = in_frame->GetSize();
  unsigned int input_data_size = sizeof(float) * size.width * size.height * in_format.GetNumPlanes();
  ncStatus_t ret = ncFifoWriteElem(input_buffers_, data, &input_data_size, 0);
  if (ret != NC_OK) {
    return common::make_errno_error(GetStringFromStatus(ret), EINVAL);
  }

  ret = ncGraphQueueInference(model->GetHandler(), &input_buffers_, 1, &output_buffers_, 1);
  if (ret != NC_OK) {
    return common::make_errno_error(GetStringFromStatus(ret), EINVAL);
  }

  unsigned int output_data_size;
  unsigned int output_data_size_byte_length = sizeof(unsigned int);
  ret =
      ncFifoGetOption(output_buffers_, NC_RO_FIFO_ELEMENT_DATA_SIZE, &output_data_size, &output_data_size_byte_length);
  if (ret != NC_OK) {
    return common::make_errno_error(GetStringFromStatus(ret), EINVAL);
  }

  float* presult = reinterpret_cast<float*>(malloc(output_data_size));
  if (!presult) {
    return common::make_errno_error("Can't alloc data for inference results", ENOMEM);
  }

  void* user_param = nullptr;
  ret = ncFifoReadElem(output_buffers_, presult, &output_data_size, &user_param);
  if (ret != NC_OK) {
    free(presult);
    return common::make_errno_error("Can't alloc data for inference results", ENOMEM);
  }

  *pred = new Prediction(presult, output_data_size);
  return common::ErrnoError();
}

Engine::~Engine() {}

}  // namespace ncsdk
}  // namespace fastoml
