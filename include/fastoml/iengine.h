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

#include <string>

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

  virtual common::ErrnoError SetProperty(const std::string& property, common::Value* value) = 0;
  virtual common::ErrnoError GetProperty(const std::string& property, common::Value** value) = 0;

  virtual common::ErrnoError MakeFrame(const common::draw::Size& size,
                                       ImageFormat::Type format,
                                       void* data,
                                       IFrame** frame) = 0;

  common::ErrnoError SetModel(IModel* in_model) WARN_UNUSED_RESULT;
  common::ErrnoError Start() WARN_UNUSED_RESULT;
  common::ErrnoError Stop() WARN_UNUSED_RESULT;
  common::ErrnoError Predict(IFrame* in_frame, IPrediction** pred) WARN_UNUSED_RESULT;

  virtual ~IEngine();

 protected:
  virtual common::ErrnoError StartImpl() = 0;
  virtual common::ErrnoError StopImpl() = 0;
  virtual common::ErrnoError PredictImpl(IFrame* in_frame, IPrediction** pred) = 0;

  IEngine();

  IModel* model_;
};

}  // namespace fastoml
