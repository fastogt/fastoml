/*  Copyright (C) 2014-2020 FastoGT. All right reserved.

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

#include <fastoml/tensorflow/engine.h>

#include <common/string_util.h>

#include <fastoml/tensorflow/frame.h>
#include <fastoml/tensorflow/model.h>
#include <fastoml/tensorflow/parameters.h>
#include <fastoml/tensorflow/prediction.h>
#include <fastoml/tensorflow/types.h>

namespace fastoml {
namespace tensorflow {

namespace {
common::ErrnoError FreeSession(TF_Session* session) {
  if (!session) {
    return common::make_errno_error_inval();
  }

  tf_status_locker_t pstatus(TF_NewStatus(), TF_DeleteStatus);
  if (pstatus) {
    TF_Status* status = pstatus.get();
    TF_CloseSession(session, status);
    if (TF_OK != TF_GetCode(status)) {
      return common::make_errno_error(TF_Message(status), EINVAL);
    }
    TF_DeleteSession(session, status);
    if (TF_OK != TF_GetCode(status)) {
      return common::make_errno_error(TF_Message(status), EINVAL);
    }
  }

  return common::ErrnoError();
}
}  // namespace

Engine::Engine() : session_(nullptr) {}

const BackendMeta Engine::meta = {TENSORFLOW, "Tensorflow", "Google's TensorFlow", TF_Version()};

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
        } else if (prop.name == INPUT_LAYER_PROPERTY) {
          common::Value::string_t data;
          if (value->GetAsString(&data)) {
            Model* model = static_cast<Model*>(model_);
            if (!model) {
              return common::make_errno_error_inval();
            }
            model->SetInputLayerName(data.as_string());
            return common::ErrnoError();
          }
        } else if (prop.name == OUTPUT_LAYER_PROPERTY) {
          common::Value::string_t data;
          if (value->GetAsString(&data)) {
            Model* model = static_cast<Model*>(model_);
            if (!model) {
              return common::make_errno_error_inval();
            }
            model->SetOutputLayerName(data.as_string());
            return common::ErrnoError();
          }
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
      } else if (prop.name == INPUT_LAYER_PROPERTY) {
        Model* model = static_cast<Model*>(model_);
        if (!model) {
          return common::make_errno_error_inval();
        }
        *value = common::Value::CreateStringValueFromBasicString(model->GetInputLayerName());
        return common::ErrnoError();
      } else if (prop.name == OUTPUT_LAYER_PROPERTY) {
        Model* model = static_cast<Model*>(model_);
        if (!model) {
          return common::make_errno_error_inval();
        }
        *value = common::Value::CreateStringValueFromBasicString(model->GetOutputLayerName());
        return common::ErrnoError();
      }
      DNOTREACHED();
      break;
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
  tf_status_locker_t pstatus(TF_NewStatus(), TF_DeleteStatus);
  if (!pstatus) {
    return common::make_errno_error("Cannot allocate status", ENOMEM);
  }

  tf_sessionoptions_locker_t popt(TF_NewSessionOptions(), TF_DeleteSessionOptions);
  if (!popt) {
    return common::make_errno_error("Cannot allocate session opt", ENOMEM);
  }

  Model* model = static_cast<Model*>(model_);
  TF_Graph* graph = model->GetGraph();
  TF_Status* status = pstatus.get();
  TF_SessionOptions* opt = popt.get();

  TF_Session* session = TF_NewSession(graph, opt, status);
  if (TF_GetCode(status) != TF_OK) {
    return common::make_errno_error(TF_Message(status), EINTR);
  }

  common::ErrnoError err = model_->Start();
  if (err) {
    FreeSession(session);
    return err;
  }

  session_ = session;
  return common::ErrnoError();
}

common::ErrnoError Engine::StopImpl() {
  if (session_) {
    common::ErrnoError err = FreeSession(session_);
    if (err) {
      return err;
    }
    session_ = nullptr;
  }

  return common::ErrnoError();
}

common::ErrnoError Engine::PredictImpl(IFrame* in_frame, IPrediction** pred) {
  Model* model = static_cast<Model*>(model_);
  /* These pointers are validated during load */
  TF_Graph* graph = model->GetGraph();
  TF_Operation* out_operation = model->GetOutputOperation();
  TF_Operation* in_operation = model->GetInputOperation();
  Frame* frame = static_cast<Frame*>(in_frame);

  TF_Tensor* in_tensor = nullptr;
  common::ErrnoError err = frame->GetOrCreateTensor(graph, in_operation, &in_tensor);
  if (err) {
    return err;
  }

  tf_status_locker_t pstatus(TF_NewStatus(), TF_DeleteStatus);
  if (!pstatus) {
    return common::make_errno_error("Cannot allocate status", ENOMEM);
  }

  TF_Status* status = pstatus.get();

  TF_Output run_outputs = {out_operation, 0};
  TF_Output run_inputs = {in_operation, 0};
  TF_Tensor* out_tensor = nullptr;

  TF_SessionRun(session_, nullptr,            /* RunOptions */
                &run_inputs, &in_tensor, 1,   /* Input tensors */
                &run_outputs, &out_tensor, 1, /* Output tensors */
                nullptr, 0,                   /* Target operations */
                nullptr,                      /* RunMetadata */
                status);
  if (TF_GetCode(status) != TF_OK) {
    return common::make_errno_error(TF_Message(status), EINTR);
  }

  Prediction* lpred = new Prediction;
  lpred->SetTensor(graph, out_operation, out_tensor);
  *pred = lpred;
  return common::ErrnoError();
}

Engine::~Engine() {}

}  // namespace tensorflow
}  // namespace fastoml
