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

#include <fastoml/tensorflow/frame.h>

#include <memory>

#include <fastoml/tensorflow/types.h>
namespace {
void DummyDeallocator(void*, size_t, void*) {}
}  // namespace

namespace fastoml {
namespace tensorflow {

common::ErrnoError MakeTensor(TF_DataType type,
                              const int64_t* dims,
                              int num_dims,
                              void* data,
                              size_t size,
                              TF_Tensor** tensor) {
  if (!tensor) {
    return common::make_errno_error_inval();
  }

  TF_Tensor* raw_tensor = TF_NewTensor(type, dims, num_dims, data, size, DummyDeallocator, nullptr);
  if (!raw_tensor) {
    return common::make_errno_error("Unable to create tensor", ENOMEM);
  }

  *tensor = raw_tensor;
  return common::ErrnoError();
}

Frame::Frame(const common::draw::Size& size, ImageFormat::Type format, data_t data)
    : base_class(size, format, data), tensor_(nullptr) {}

common::ErrnoError Frame::GetTensorShape(TF_Graph* graph,
                                         TF_Operation* operation,
                                         TF_DataType* type,
                                         int64_t** dims,
                                         int* num_dims,
                                         size_t* size) {
  if (!graph) {
    return common::make_errno_error("Attempting to validate frame with NULL graph", EINVAL);
  }

  if (!operation) {
    return common::make_errno_error("Attempting to validate frame with NULL operation", EINVAL);
  }

  TF_Output output = {operation, 0};
  tf_status_locker_t pstatus(TF_NewStatus(), TF_DeleteStatus);
  if (!pstatus) {
    return common::make_errno_error("Cannot allocate status", ENOMEM);
  }

  TF_Status* status = pstatus.get();
  int lnum_dims = TF_GraphGetTensorNumDims(graph, output, status);
  if (TF_GetCode(status) != TF_OK) {
    return common::make_errno_error(TF_Message(status), EINTR);
  }

  int64_t* ldims = new int64_t[lnum_dims];
  TF_GraphGetTensorShape(graph, output, ldims, lnum_dims, status);
  if (TF_GetCode(status) != TF_OK) {
    delete[] ldims;
    return common::make_errno_error(TF_Message(status), EINTR);
  }

  ldims[0] = 1;

  TF_DataType ltype = TF_OperationOutputType(output);
  size_t type_size = TF_DataTypeSize(ltype);
  size_t data_size = GetRequiredBufferSize(ldims, lnum_dims, type_size);

  *type = ltype;
  *dims = ldims;
  *num_dims = lnum_dims;
  *size = data_size;
  return common::ErrnoError();
}

common::ErrnoError Frame::Validate(int64_t* dims, int64_t num_dims) {
  if (num_dims <= 3) {
    return common::make_errno_error_inval();
  }

  /* We only support 1 batch */
  if (dims[0] != 1) {
    return common::make_errno_error("We only support a batch of 1 image(s) in our frames", EINVAL);
  }

  common::draw::Size size = GetSize();
  /* Check that widths match */
  if (dims[1] != size.width) {
    return common::make_errno_error("Unsupported image width", EINVAL);
  }

  /* Check that heights match */
  if (dims[2] != size.height) {
    return common::make_errno_error("Unsupported image height", EINVAL);
  }

  /* Check that channels match
   * TODO: relate this to the input format
   */
  if (dims[3] != 3) {
    return common::make_errno_error("We only support a 3 channels per image", EINVAL);
  }

  return common::ErrnoError();
}

common::ErrnoError Frame::GetOrCreateTensor(TF_Graph* graph, TF_Operation* operation, TF_Tensor** tensor) {
  if (!tensor || !graph || !operation) {
    return common::make_errno_error_inval();
  }

  if (tensor_) {
    *tensor = tensor_;
    return common::ErrnoError();
  }

  TF_DataType type;
  int64_t* dims = nullptr;
  int num_dims = 0;
  size_t size = 0;
  common::ErrnoError err = GetTensorShape(graph, operation, &type, &dims, &num_dims, &size);
  if (err) {
    return err;
  }

  std::unique_ptr<int64_t[]> pldims(dims);
  err = Validate(dims, num_dims);
  if (err) {
    return err;
  }

  TF_Tensor* ltensor = nullptr;
  err = MakeTensor(type, dims, num_dims, GetData(), size, &ltensor);
  if (err) {
    return err;
  }

  tensor_ = ltensor;
  *tensor = tensor_;
  return common::ErrnoError();
}

Frame::~Frame() {
  if (tensor_) {
    TF_DeleteTensor(tensor_);
    tensor_ = nullptr;
  }
}

}  // namespace tensorflow
}  // namespace fastoml
