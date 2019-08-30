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

#include <fastoml/ncsdk/engine.h>

#include <common/string_util.h>

#include <fastoml/ncsdk/frame.h>
#include <fastoml/ncsdk/model.h>
#include <fastoml/ncsdk/parameters.h>
#include <fastoml/ncsdk/prediction.h>
#include <fastoml/ncsdk/types.h>

namespace fastoml {
namespace ncsdk {

Engine::Engine() {}

const BackendMeta Engine::meta = {NCSDK, "NCSDK", "NVidia", "1"};

BackendMeta Engine::GetBackendMeta() const {
  return meta;
}

common::ErrnoError Engine::SetProperty(const std::string& property, common::Value* value) {
  if (property.empty() || !value) {
    return common::make_errno_error_inval();
  }

  for (size_t i = 0; i < kParameters.size(); ++i) {
    const ParameterMeta prop = kParameters[i];
    if (common::EqualsASCII(prop.name, property, false)) {
      if (prop.type == value->GetType()) {
        if (prop.name == VERSION_PROPERTY) {
          return common::make_errno_error("Property is not writeable", EINVAL);
        }
      }

      return common::make_errno_error("Invalid value type", EINVAL);
    }
  }

  return common::make_errno_error("Propery not found", EINVAL);
}

common::ErrnoError Engine::GetProperty(const std::string& property, common::Value** value) {
  if (property.empty() || !value) {
    return common::make_errno_error_inval();
  }

  for (size_t i = 0; i < kParameters.size(); ++i) {
    const ParameterMeta prop = kParameters[i];
    if (common::EqualsASCII(prop.name, property, false)) {
      if (prop.name == VERSION_PROPERTY) {
        *value = common::Value::CreateStringValueFromBasicString(TF_Version());
        return common::ErrnoError();
      }
    }
  }

  return common::make_errno_error("Propery not found", EINVAL);
}

common::ErrnoError Engine::MakeFrame(const common::draw::Size& size,
                                     ImageFormat::Type format,
                                     void* data,
                                     IFrame** frame) {
  if (!frame) {
    return common::make_errno_error_inval();
  }

  *frame = new Frame(size, format, data);
  return common::ErrnoError();
}

common::ErrnoError Engine::StartImpl() {
  return common::ErrnoError();
}

common::ErrnoError Engine::StopImpl() {
  return common::ErrnoError();
}

common::ErrnoError Engine::PredictImpl(IFrame* in_frame, IPrediction** pred) {
  return common::ErrnoError();
}

Engine::~Engine() {}

}  // namespace ncsdk
}  // namespace fastoml
