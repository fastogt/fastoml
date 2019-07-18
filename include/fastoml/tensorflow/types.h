/*  Copyright (C) 2014-2019 FastoGT. All right reserved.

    This file is part of FastoTV.

    FastoTV is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    FastoTV is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with FastoTV. If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include <tensorflow/c/c_api.h>
#include <memory>

namespace fastoml {
namespace tensorflow {

typedef const std::unique_ptr<TF_Status, decltype(&TF_DeleteStatus)> tf_status_locker_t;

size_t GetRequiredBufferSize(int64_t* dims, int64_t num_dims, size_t type_size);
}  // namespace tensorflow
}  // namespace fastoml
