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

#include <fastoml/backend.h>

#include <fastoml/tensorflow/engine.h>
#include <fastoml/tensorflow/model.h>
#include <fastoml/tensorflow/parameters.h>

namespace fastoml {

common::Error BackEnd::GetParameters(SupportedBackends code, const std::vector<ParameterMeta>** params) {
  if (code == MAX_BACKENDS || !params) {
    return common::make_error_inval();
  }

  *params = &tensorflow::kParameters;
  return common::Error();
}

BackEnd::BackEnd() : engine_(nullptr), model_(nullptr) {}

BackendMeta BackEnd::GetMeta() const {
  return engine_->GetBackendMeta();
}

common::Error BackEnd::SetProperty(const std::string& property, common::Value* value) {
  return engine_->SetProperty(property, value);
}

common::Error BackEnd::GetProperty(const std::string& property, common::Value** value) {
  return engine_->GetProperty(property, value);
}

common::Error BackEnd::MakeFrame(const common::draw::Size& size, ImageFormat::Type format, void* data, IFrame** frame) {
  return engine_->MakeFrame(size, format, data, frame);
}

common::Error BackEnd::LoadGraph(const common::file_system::ascii_file_string_path& path) {
  return model_->Load(path);
}

common::Error BackEnd::Start() {
  return engine_->Start();
}

common::Error BackEnd::Predict(IFrame* in_frame, IPrediction** pred) {
  return engine_->Predict(in_frame, pred);
}

common::Error BackEnd::Stop() {
  return engine_->Stop();
}

common::Error BackEnd::MakeBackEnd(SupportedBackends code, BackEnd** backend) {
  if (code == MAX_BACKENDS || !backend) {
    return common::make_error_inval();
  }

  fastoml::IModel* model = new fastoml::tensorflow::Model;
  fastoml::IEngine* engine = new fastoml::tensorflow::Engine;
  common::Error err = engine->SetModel(model);
  if (err) {
    return err;
  }

  BackEnd* back = new BackEnd;
  back->model_ = model;
  back->engine_ = engine;
  *backend = back;
  return common::Error();
}

BackEnd::~BackEnd() {
  destroy(&engine_);
  destroy(&model_);
}

}  // namespace fastoml
