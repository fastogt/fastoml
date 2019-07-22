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

#include <string>
#include <vector>

#include <common/error.h>
#include <common/file_system/path.h>
#include <common/value.h>

#include <fastoml/backend_meta.h>
#include <fastoml/iframe.h>
#include <fastoml/iprediction.h>
#include <fastoml/parameter_meta.h>

namespace fastoml {

class IEngine;
class IModel;

class Backend final {
 public:
  enum State { STARTED, STOPPED };

  static common::ErrnoError MakeBackEnd(SupportedBackends code, Backend** backend);
  static common::ErrnoError GetParameters(SupportedBackends code, const std::vector<ParameterMeta>** params);
  static common::ErrnoError GetMeta(SupportedBackends code, const BackendMeta** meta);

  BackendMeta GetMeta() const;
  common::ErrnoError SetProperty(const std::string& property, common::Value* value);
  common::ErrnoError GetProperty(const std::string& property, common::Value** value);

  common::ErrnoError MakeFrame(const common::draw::Size& size, ImageFormat::Type format, void* data, IFrame** frame)
      WARN_UNUSED_RESULT;

  common::ErrnoError LoadGraph(const common::file_system::ascii_file_string_path& path) WARN_UNUSED_RESULT;
  common::ErrnoError Start() WARN_UNUSED_RESULT;
  common::ErrnoError Predict(IFrame* in_frame, IPrediction** pred) WARN_UNUSED_RESULT;

  common::ErrnoError Stop() WARN_UNUSED_RESULT;

  ~Backend();

 protected:
  Backend();

 private:
  IEngine* engine_;
  IModel* model_;
  State state_;

  DISALLOW_COPY_AND_ASSIGN(Backend);
};

}  // namespace fastoml
