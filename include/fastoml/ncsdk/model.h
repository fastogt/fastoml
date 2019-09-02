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

#include <fastoml/imodel.h>

struct ncGraphHandle_t;

namespace fastoml {
namespace ncsdk {

class Model : public IModel {
 public:
  Model();

  common::ErrnoError Load(const common::file_system::ascii_file_string_path& path) override WARN_UNUSED_RESULT;
  common::ErrnoError Start() override WARN_UNUSED_RESULT;

  void SetName(const std::string& name);
  std::string GetName() const;

  ~Model() override;

  private:
    ncGraphHandle_t* graph_;
    void* graph_data_;
    size_t graph_size_;
    std::string name_;
};

}  // namespace ncsdk
}  // namespace fastoml
