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

#include <fastoml/iprediction.h>

#include <tensorflow/c/c_api.h>

namespace fastoml {
namespace tensorflow {

class Prediction : public IPrediction {
 public:
  Prediction();

  virtual common::Error At(size_t index, float* val) override;
  virtual void* GetResultData() override;
  virtual size_t GetResultSize() const override;

  common::Error SetTensor(TF_Graph* graph, TF_Operation* operation, TF_Tensor* tensor);

  virtual ~Prediction() override;

 private:
  TF_Tensor* tensor_;
  size_t result_size_;
};

}  // namespace tensorflow
}  // namespace fastoml
