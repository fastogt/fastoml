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

#include <common/error.h>

#include <fastoml/iengine.h>

namespace fastoml {
namespace tensorflow {

class Engine : public IEngine {
 public:
  Engine();

  virtual BackendMeta GetBackendMeta() const override;

  virtual common::Error SetProperty(const std::string& property, common::Value* value) override;
  virtual common::Error GetProperty(const std::string& property, common::Value** value) override;

  virtual common::Error SetModel(IModel* in_model) override;
  virtual common::Error Start() override;
  virtual common::Error Stop() override;
  virtual common::Error Predict(IFrame* in_frame, IPrediction** pred) override;

  virtual ~Engine() override;

 private:
  enum State { STARTED, STOPPED };
  State state_;

  TF_Session* session_;
  IModel* model_;
};

}  // namespace tensorflow
}  // namespace fastoml
