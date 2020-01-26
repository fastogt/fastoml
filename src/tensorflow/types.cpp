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

#include <fastoml/tensorflow/types.h>

namespace fastoml {
namespace tensorflow {

size_t GetRequiredBufferSize(const int64_t* dims, int64_t num_dims, size_t type_size) {
  size_t size = 1;

  /* For each dimension, multiply the amount of entries */
  for (int64_t dim = 0; dim < num_dims; ++dim) {
    size *= dims[dim];
  }

  return size * type_size;
}

}  // namespace tensorflow
}  // namespace fastoml
