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

#include <common/error.h>
#include <common/file_system/path.h>
#include <common/value.h>

#include <fastoml/backend_meta.h>
#include <fastoml/parameter_meta.h>

namespace fastoml {

class IEngine;
class IModel;

class BackEnd final {
 public:
  static common::Error MakeBackEnd(SupportedBackends code, BackEnd** backend);
  static common::Error GetParameters(SupportedBackends code, const std::vector<ParameterMeta>** params);

  BackendMeta GetMeta() const;
  common::Error SetProperty(const std::string& property, common::Value* value);
  common::Error GetProperty(const std::string& property, common::Value** value);

  common::Error LoadGraph(const common::file_system::ascii_file_string_path& path) WARN_UNUSED_RESULT;
  common::Error Start() WARN_UNUSED_RESULT;
  common::Error Stop() WARN_UNUSED_RESULT;

  ~BackEnd();

 protected:
  BackEnd();

 private:
  IEngine* engine_;
  IModel* model_;

  DISALLOW_COPY_AND_ASSIGN(BackEnd);
};

}  // namespace fastoml
