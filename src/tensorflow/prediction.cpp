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

#include <fastoml/tensorflow/prediction.h>

#include <fastoml/tensorflow/types.h>

namespace fastoml {
namespace tensorflow {

Prediction::Prediction() : tensor_(nullptr), result_size_(0) {}

common::ErrnoError Prediction::At(size_t index, float* val) {
  if (!val) {
    return common::make_errno_error_inval();
  }

  if (!tensor_) {
    return common::make_errno_error("Prediction was not properly initialized", EINVAL);
  }

  size_t n_results = GetResultSize() / sizeof(float);
  if (n_results < index) {
    return common::make_errno_error("Triying to access an non-existing index", EINVAL);
  }

  float* fdata = static_cast<float*>(GetResultData());
  if (!fdata) {
    return common::make_errno_error("Prediction result not set yet", ENOMEM);
  }

  *val = fdata[index];
  return common::ErrnoError();
}

void* Prediction::GetResultData() {
  if (!tensor_) {
    return nullptr;
  }

  return TF_TensorData(tensor_);
}

size_t Prediction::GetResultSize() const {
  return result_size_;
}

common::ErrnoError Prediction::SetTensor(TF_Graph* graph, TF_Operation* operation, TF_Tensor* tensor) {
  if (!graph || !operation || !tensor) {
    return common::make_errno_error_inval();
  }

  TF_Output output = {operation, 0};
  tf_status_locker_t pstatus(TF_NewStatus(), TF_DeleteStatus);
  if (!pstatus) {
    return common::make_errno_error("Cannot allocate status", EINVAL);
  }

  TF_Status* status = pstatus.get();
  int lnum_dims = TF_GraphGetTensorNumDims(graph, output, status);
  if (TF_GetCode(status) != TF_OK) {
    return common::make_errno_error(TF_Message(status), EINTR);
  }

  std::unique_ptr<int64_t[]> pldims(new int64_t[lnum_dims]);
  int64_t* ldims = pldims.get();
  TF_GraphGetTensorShape(graph, output, ldims, lnum_dims, status);
  if (TF_GetCode(status) != TF_OK) {
    return common::make_errno_error(TF_Message(status), EINTR);
  }

  ldims[0] = 1;

  TF_DataType type = TF_OperationOutputType(output);
  size_t type_size = TF_DataTypeSize(type);
  size_t data_size = GetRequiredBufferSize(ldims, lnum_dims, type_size);

  if (TF_FLOAT != type) {
    return common::make_errno_error("The output of this model is not floating point", EINTR);
  }

  tensor_ = tensor;
  result_size_ = data_size;
  return common::ErrnoError();
}

Prediction::~Prediction() {
  if (tensor_) {
    TF_DeleteTensor(tensor_);
    tensor_ = nullptr;
  }
}

}  // namespace tensorflow
}  // namespace fastoml
