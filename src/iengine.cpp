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

#include <fastoml/iengine.h>

namespace fastoml {

IEngine::IEngine() : model_(nullptr) {}

common::ErrnoError IEngine::SetModel(IModel* in_model) {
  if (!in_model) {
    return common::make_errno_error("Received null model", EINVAL);
  }

  if (model_) {
    return common::make_errno_error("Changing models not supported", EINVAL);
  }

  model_ = in_model;
  return common::ErrnoError();
}

common::ErrnoError IEngine::Start() {
  if (!model_) {
    return common::make_errno_error("Model not set yet", EINVAL);
  }

  return StartImpl();
}

common::ErrnoError IEngine::Stop() {
  if (!model_) {
    return common::make_errno_error("Model not set yet", EINVAL);
  }

  return StopImpl();
}

common::ErrnoError IEngine::Predict(IFrame* in_frame, IPrediction** pred) {
  if (!in_frame || !pred) {
    return common::make_errno_error_inval();
  }

  if (!model_) {
    return common::make_errno_error("Model not set yet", EINVAL);
  }

  return PredictImpl(in_frame, pred);
}

IEngine::~IEngine() {}

}  // namespace fastoml
