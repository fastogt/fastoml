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

#include <fastoml/imodel.h>

namespace fastoml {
namespace tensorflow {

class Model : public IModel {
 public:
  Model();

  virtual common::Error Load(const common::file_system::ascii_file_string_path& path) override WARN_UNUSED_RESULT;
  virtual common::Error Start() override WARN_UNUSED_RESULT;
  virtual ~Model() override;

  std::string GetInputLayerName() const;
  void SetInputLayerName(const std::string& name);

  std::string GetOutputLayerName() const;
  void SetOutputLayerName(const std::string& name);

  TF_Graph* GetGraph() const;

  TF_Operation* GetOutputOperation() const;
  TF_Operation* GetInputOperation() const;

 private:
  common::Error Load(TF_Buffer* buffer);

  TF_Graph* graph_;
  TF_Buffer* buffer_;
  TF_Operation* in_operation_;
  TF_Operation* out_operation_;
  std::string input_layer_name_;
  std::string output_layer_name_;
};

}  // namespace tensorflow
}  // namespace fastoml