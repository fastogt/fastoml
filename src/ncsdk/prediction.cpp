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

Prediction::Prediction() : result_size_(0) {}

common::ErrnoError Prediction::At(size_t index, float* val) {
  return common::ErrnoError();
}

void* Prediction::GetResultData() {
  return nullptr;
}

size_t Prediction::GetResultSize() const {
  return result_size_;
}

Prediction::~Prediction() {}

}  // namespace ncsdk
}  // namespace fastoml
