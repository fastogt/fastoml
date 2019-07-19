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

#include <common/draw/types.h>
#include <common/error.h>
#include <common/value.h>

#include <fastoml/backend_meta.h>

namespace fastoml {

class IFrame;
class IModel;
class IPrediction;

class IEngine {
 public:
  virtual BackendMeta GetBackendMeta() const = 0;

  virtual common::Error SetProperty(const std::string& property, common::Value* value) = 0;
  virtual common::Error GetProperty(const std::string& property, common::Value** value) = 0;

  virtual common::Error MakeFrame(const common::draw::Size& size,
                                  ImageFormat::Type format,
                                  void* data,
                                  IFrame** frame) = 0;

  virtual common::Error SetModel(IModel* in_model) = 0;
  virtual common::Error Start() = 0;
  virtual common::Error Stop() = 0;
  virtual common::Error Predict(IFrame* in_frame, IPrediction** pred) = 0;
  virtual ~IEngine();
};

}  // namespace fastoml
