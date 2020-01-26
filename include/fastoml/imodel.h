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

#pragma once

#include <common/error.h>

#include <common/file_system/path.h>

namespace fastoml {

class IModel {
 public:
  virtual common::ErrnoError Load(const common::file_system::ascii_file_string_path& path) WARN_UNUSED_RESULT = 0;
  virtual common::ErrnoError Start() WARN_UNUSED_RESULT = 0;
  virtual common::ErrnoError Stop() WARN_UNUSED_RESULT = 0;
  virtual ~IModel();
};

}  // namespace fastoml
