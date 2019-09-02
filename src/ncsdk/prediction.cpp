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

#include <fastoml/ncsdk/prediction.h>

#include <fastoml/ncsdk/types.h>

namespace fastoml {
namespace ncsdk {

Prediction::Prediction(float* data, size_t size) : result_data_(data), result_size_(size) {}

common::ErrnoError Prediction::At(size_t index, float* val) {
  if (!val) {
    return common::make_errno_error_inval();
  }

  if (!result_data_) {
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
  return result_data_;
}

size_t Prediction::GetResultSize() const {
  return result_size_;
}

Prediction::~Prediction() {
  if (result_data_) {
    free(result_data_);
    result_data_ = nullptr;
  }
  result_size_ = 0;
}

}  // namespace ncsdk
}  // namespace fastoml
