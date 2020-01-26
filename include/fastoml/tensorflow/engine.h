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

#include <tensorflow/c/c_api.h>

#include <fastoml/iengine.h>

namespace fastoml {
namespace tensorflow {

class Engine : public IEngine {
 public:
  Engine();
  static const BackendMeta meta;

  BackendMeta GetBackendMeta() const override;

  common::ErrnoError SetProperty(const std::string& property, common::Value* value) override;
  common::ErrnoError GetProperty(const std::string& property, common::Value** value) override;

  common::ErrnoError MakeFrame(const common::draw::Size& size,
                               ImageFormat::Type format,
                               void* data,
                               IFrame** frame) override;

  ~Engine() override;

 protected:
  common::ErrnoError StartImpl() override;
  common::ErrnoError StopImpl() override;
  common::ErrnoError PredictImpl(IFrame* in_frame, IPrediction** pred) override;

 private:
  TF_Session* session_;
};

}  // namespace tensorflow
}  // namespace fastoml
