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

#pragma once

#include <tensorflow/c/c_api.h>

#include <fastoml/imodel.h>

namespace fastoml {
namespace ncsdk {

class Model : public IModel {
 public:
  Model();

  common::ErrnoError Load(const common::file_system::ascii_file_string_path& path) override WARN_UNUSED_RESULT;
  common::ErrnoError Start() override WARN_UNUSED_RESULT;
  ~Model() override;
};

}  // namespace ncsdk
}  // namespace fastoml
