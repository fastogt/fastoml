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

#include <fastoml/tensorflow/engine.h>

#include <common/string_util.h>

#include <fastoml/tensorflow/frame.h>
#include <fastoml/tensorflow/model.h>
#include <fastoml/tensorflow/parameters.h>
#include <fastoml/tensorflow/prediction.h>
#include <fastoml/tensorflow/types.h>

namespace fastoml {
namespace tensorflow {

Engine::Engine() : state_(STOPPED), session_(nullptr), model_(nullptr) {}

BackendMeta Engine::GetBackendMeta() const {
  return {TENSORFLOW, "Tensorflow", "Google's TensorFlow", TF_Version()};
}

common::Error Engine::SetProperty(const std::string& property, common::Value* value) {
  if (property.empty() || !value) {
    return common::make_error_inval();
  }

  for (size_t i = 0; i < kParameters.size(); ++i) {
    const ParameterMeta prop = kParameters[i];
    if (common::EqualsASCII(prop.name, property, false)) {
      if (prop.type == value->GetType()) {
        if (prop.name == VERSION_PROPERTY) {
          return common::make_error("Property is not writeable");
        } else if (prop.name == INPUT_LAYER_PROPERTY) {
          common::Value::string_t data;
          if (value->GetAsString(&data)) {
            Model* model = static_cast<Model*>(model_);
            if (!model) {
              return common::make_error_inval();
            }
            model->SetInputLayerName(data.as_string());
            return common::Error();
          }
        } else if (prop.name == OUTPUT_LAYER_PROPERTY) {
          common::Value::string_t data;
          if (value->GetAsString(&data)) {
            Model* model = static_cast<Model*>(model_);
            if (!model) {
              return common::make_error_inval();
            }
            model->SetOutputLayerName(data.as_string());
            return common::Error();
          }
        }
      }

      return common::make_error("Invalid value type");
    }
  }

  return common::make_error("Propery not found");
}

common::Error Engine::GetProperty(const std::string& property, common::Value** value) {
  if (property.empty() || !value) {
    return common::make_error_inval();
  }

  for (size_t i = 0; i < kParameters.size(); ++i) {
    const ParameterMeta prop = kParameters[i];
    if (common::EqualsASCII(prop.name, property, false)) {
      if (prop.name == VERSION_PROPERTY) {
        *value = common::Value::CreateStringValueFromBasicString(TF_Version());
        return common::Error();
      } else if (prop.name == INPUT_LAYER_PROPERTY) {
        Model* model = static_cast<Model*>(model_);
        if (!model) {
          return common::make_error_inval();
        }
        *value = common::Value::CreateStringValueFromBasicString(model->GetInputLayerName());
        return common::Error();
      } else if (prop.name == OUTPUT_LAYER_PROPERTY) {
        Model* model = static_cast<Model*>(model_);
        if (!model) {
          return common::make_error_inval();
        }
        *value = common::Value::CreateStringValueFromBasicString(model->GetOutputLayerName());
        return common::Error();
      }
      DNOTREACHED();
      break;
    }
  }

  return common::make_error("Propery not found");
}

common::Error Engine::SetModel(IModel* in_model) {
  if (!in_model) {
    return common::make_error("Received null model");
  }

  if (state_ != State::STOPPED) {
    return common::make_error("Stop model before setting a new state");
  }

  if (model_) {
    return common::make_error("Changing models not supported");
  }

  model_ = in_model;
  return common::Error();
}

common::Error Engine::Start() {
  if (state_ == State::STARTED) {
    return common::make_error("Engine already started");
  }

  if (!model_) {
    return common::make_error("Model not set yet");
  }

  tf_status_locker_t pstatus(TF_NewStatus(), TF_DeleteStatus);
  if (!pstatus) {
    return common::make_error("Cannot allocate status");
  }

  tf_sessionoptions_locker_t popt(TF_NewSessionOptions(), TF_DeleteSessionOptions);
  if (!popt) {
    return common::make_error("Cannot allocate session opt");
  }

  Model* model = static_cast<Model*>(model_);
  TF_Graph* graph = model->GetGraph();
  TF_Status* status = pstatus.get();
  TF_SessionOptions* opt = popt.get();

  TF_Session* session = TF_NewSession(graph, opt, status);
  if (TF_GetCode(status) != TF_OK) {
    return common::make_error(TF_Message(status));
  }

  common::Error err = model_->Start();
  if (err) {
    return err;
  }

  session_ = session;
  state_ = State::STARTED;
  return common::Error();
}

common::Error Engine::Stop() {
  if (state_ == State::STOPPED) {
    return common::make_error("Engine already stopped");
  }

  state_ = State::STOPPED;
  return common::Error();
}

common::Error Engine::Predict(IFrame* in_frame, IPrediction** pred) {
  if (!in_frame || !pred) {
    return common::make_error_inval();
  }

  if (state_ != State::STARTED) {
    return common::make_error("Engine not started");
  }

  Model* model = static_cast<Model*>(model_);
  /* These pointers are validated during load */
  TF_Graph* pgraph = model->GetGraph();
  TF_Operation* out_operation = model->GetOutputOperation();
  TF_Operation* in_operation = model->GetInputOperation();
  Frame* frame = static_cast<Frame*>(in_frame);

  TF_Tensor* pin_tensor = nullptr;
  common::Error err = frame->GetOrCreateTensor(pgraph, in_operation, &pin_tensor);
  if (err) {
    return err;
  }

  tf_status_locker_t pstatus(TF_NewStatus(), TF_DeleteStatus);
  if (!pstatus) {
    return common::make_error("Cannot allocate status");
  }

  TF_Tensor* in_tensor = pin_tensor;
  Prediction* lpred = new Prediction;
  TF_Status* status = pstatus.get();

  TF_Output run_outputs = {out_operation, 0};
  TF_Output run_inputs = {in_operation, 0};
  TF_Tensor* out_tensor = nullptr;

  TF_SessionRun(session_, nullptr,            /* RunOptions */
                &run_inputs, &in_tensor, 1,   /* Input tensors */
                &run_outputs, &out_tensor, 1, /* Output tensors */
                nullptr, 0,                   /* Target operations */
                nullptr,                      /* RunMetadata */
                status);
  if (TF_GetCode(status) != TF_OK) {
    return common::make_error(TF_Message(status));
  }

  lpred->SetTensor(pgraph, out_operation, out_tensor);
  *pred = lpred;
  return common::Error();
}

Engine::~Engine() {}

}  // namespace tensorflow
}  // namespace fastoml
