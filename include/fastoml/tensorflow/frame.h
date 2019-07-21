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

#include <fastoml/iframe.h>

#include <tensorflow/c/c_api.h>

namespace fastoml {
namespace tensorflow {

common::ErrnoError MakeTensor(TF_DataType type,
                              const int64_t* dims,
                              int num_dims,
                              void* data,
                              size_t size,
                              TF_Tensor** tensor);

class Frame : public IFrame {
 public:
  typedef IFrame base_class;
  Frame(const common::draw::Size& size, ImageFormat::Type format, data_t data);

  common::ErrnoError GetOrCreateTensor(TF_Graph* graph, TF_Operation* operation, TF_Tensor** tensor);

  virtual ~Frame() override;

 private:
  common::ErrnoError GetTensorShape(TF_Graph* graph,
                                    TF_Operation* operation,
                                    TF_DataType* type,
                                    int64_t** dims,
                                    int* num_dims,
                                    size_t* size);
  common::ErrnoError Validate(int64_t* dims, int64_t num_dims);

  TF_Tensor* tensor_;
};

}  // namespace tensorflow
}  // namespace fastoml
