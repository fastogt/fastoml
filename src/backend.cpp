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

#include <fastoml/backend.h>

#include <fastoml/tensorflow/engine.h>
#include <fastoml/tensorflow/model.h>
#include <fastoml/tensorflow/parameters.h>

namespace fastoml {

common::Error Backend::GetParameters(SupportedBackends code, const std::vector<ParameterMeta>** params) {
  if (!params) {
    return common::make_error_inval();
  }

  *params = &tensorflow::kParameters;
  return common::Error();
}

common::Error Backend::GetMeta(SupportedBackends code, const BackendMeta** meta) {
  if (!meta) {
    return common::make_error_inval();
  }

  if (code == TENSORFLOW) {
    *meta = &tensorflow::Engine::meta;
  }

  return common::make_error_inval();
}

Backend::Backend() : engine_(nullptr), model_(nullptr) {}

BackendMeta Backend::GetMeta() const {
  return engine_->GetBackendMeta();
}

common::Error Backend::SetProperty(const std::string& property, common::Value* value) {
  return engine_->SetProperty(property, value);
}

common::Error Backend::GetProperty(const std::string& property, common::Value** value) {
  return engine_->GetProperty(property, value);
}

common::Error Backend::MakeFrame(const common::draw::Size& size, ImageFormat::Type format, void* data, IFrame** frame) {
  return engine_->MakeFrame(size, format, data, frame);
}

common::Error Backend::LoadGraph(const common::file_system::ascii_file_string_path& path) {
  return model_->Load(path);
}

common::Error Backend::Start() {
  return engine_->Start();
}

common::Error Backend::Predict(IFrame* in_frame, IPrediction** pred) {
  return engine_->Predict(in_frame, pred);
}

common::Error Backend::Stop() {
  return engine_->Stop();
}

common::Error Backend::MakeBackEnd(SupportedBackends code, Backend** backend) {
  if (!backend) {
    return common::make_error_inval();
  }

  fastoml::IModel* model = new fastoml::tensorflow::Model;
  fastoml::IEngine* engine = new fastoml::tensorflow::Engine;
  common::Error err = engine->SetModel(model);
  if (err) {
    return err;
  }

  Backend* back = new Backend;
  back->model_ = model;
  back->engine_ = engine;
  *backend = back;
  return common::Error();
}

Backend::~Backend() {
  destroy(&engine_);
  destroy(&model_);
}

}  // namespace fastoml
