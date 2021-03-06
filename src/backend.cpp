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

#include <fastoml/backend.h>

#include <fastoml/iengine.h>
#include <fastoml/imodel.h>

#if defined(HAVE_TENSORFLOW)
#include <fastoml/tensorflow/engine.h>
#include <fastoml/tensorflow/model.h>
#include <fastoml/tensorflow/parameters.h>
#endif

#if defined(HAVE_NCSDK)
#include <fastoml/ncsdk/engine.h>
#include <fastoml/ncsdk/model.h>
#include <fastoml/ncsdk/parameters.h>
#endif

namespace fastoml {

common::ErrnoError Backend::GetParameters(SupportedBackends code, const std::vector<ParameterMeta>** params) {
  if (!params) {
    return common::make_errno_error_inval();
  }

  if (code == TENSORFLOW) {
#if defined(HAVE_TENSORFLOW)
    *params = &tensorflow::kParameters;
#else
    return common::make_errno_error_inval();
#endif
  } else if (code == NCSDK) {
#if defined(HAVE_NCSDK)
    *params = &ncsdk::kParameters;
#else
    return common::make_errno_error_inval();
#endif
  }
  return common::ErrnoError();
}

common::ErrnoError Backend::GetMeta(SupportedBackends code, const BackendMeta** meta) {
  if (!meta) {
    return common::make_errno_error_inval();
  }

  if (code == TENSORFLOW) {
#if defined(HAVE_TENSORFLOW)
    *meta = &tensorflow::Engine::meta;
#else
    return common::make_errno_error_inval();
#endif
  } else if (code == NCSDK) {
#if defined(HAVE_NCSDK)
    *meta = &ncsdk::Engine::meta;
#else
    return common::make_errno_error_inval();
#endif
  }

  return common::ErrnoError();
}

Backend::Backend() : engine_(nullptr), model_(nullptr), state_(STOPPED) {}

BackendMeta Backend::GetMeta() const {
  return engine_->GetBackendMeta();
}

common::ErrnoError Backend::SetProperty(const std::string& property, common::Value* value) {
  return engine_->SetProperty(property, value);
}

common::ErrnoError Backend::GetProperty(const std::string& property, common::Value** value) {
  return engine_->GetProperty(property, value);
}

common::ErrnoError Backend::MakeFrame(const common::draw::Size& size,
                                      ImageFormat::Type format,
                                      void* data,
                                      IFrame** frame) {
  return engine_->MakeFrame(size, format, data, frame);
}

common::ErrnoError Backend::LoadGraph(const common::file_system::ascii_file_string_path& path) {
  if (state_ == STARTED) {
    return common::ErrnoError();
  }

  return model_->Load(path);
}

common::ErrnoError Backend::Start() {
  if (state_ == State::STARTED) {
    return common::ErrnoError();
  }

  common::ErrnoError err = engine_->Start();
  if (err) {
    return err;
  }

  state_ = State::STARTED;
  return common::ErrnoError();
}

common::ErrnoError Backend::Predict(IFrame* in_frame, IPrediction** pred) {
  if (state_ != State::STARTED) {
    return common::make_errno_error("Backend not started", EINTR);
  }

  return engine_->Predict(in_frame, pred);
}

common::ErrnoError Backend::Stop() {
  if (state_ == State::STOPPED) {
    return common::ErrnoError();
  }

  common::ErrnoError err = engine_->Stop();
  if (err) {
    return err;
  }

  state_ = State::STOPPED;
  return common::ErrnoError();
}

common::ErrnoError Backend::MakeBackEnd(SupportedBackends code, Backend** backend) {
  if (!backend) {
    return common::make_errno_error_inval();
  }

  fastoml::IModel* model = nullptr;
  fastoml::IEngine* engine = nullptr;
  if (code == TENSORFLOW) {
#if defined(HAVE_TENSORFLOW)
    model = new fastoml::tensorflow::Model;
    engine = new fastoml::tensorflow::Engine;
#else
    return common::make_errno_error_inval();
#endif
  } else if (code == NCSDK) {
#if defined(HAVE_NCSDK)
    model = new fastoml::ncsdk::Model;
    engine = new fastoml::ncsdk::Engine;
#else
    return common::make_errno_error_inval();
#endif
  }

  common::ErrnoError err = engine->SetModel(model);
  if (err) {
    return err;
  }

  Backend* back = new Backend;
  back->model_ = model;
  back->engine_ = engine;
  *backend = back;
  return common::ErrnoError();
}

Backend::~Backend() {
  destroy(&engine_);
  destroy(&model_);
}

}  // namespace fastoml
