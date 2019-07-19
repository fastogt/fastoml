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

#include <fastoml/tensorflow/model.h>

#include <common/file_system/file.h>

#include <fastoml/tensorflow/types.h>

namespace {
void free_buffer(void* data, size_t) {
  free(data);
}
}  // namespace

namespace fastoml {
namespace tensorflow {

Model::Model()
    : graph_(nullptr),
      buffer_(nullptr),
      in_operation_(nullptr),
      out_operation_(nullptr),
      input_layer_name_(),
      output_layer_name_() {}

common::Error Model::Load(const common::file_system::ascii_file_string_path& path) {
  if (!path.IsValid()) {
    return common::make_error_inval();
  }

  common::file_system::ANSIFile file;
  common::ErrnoError errn = file.Open(path, "rb");
  if (errn) {
    return common::make_error_from_errno(errn);
  }

  size_t file_size;
  errn = file.GetSize(&file_size);
  if (errn) {
    file.Close();
    return common::make_error_from_errno(errn);
  }

  common::byte_t* data = reinterpret_cast<common::byte_t*>(malloc(file_size));
  if (!data) {
    file.Close();
    return common::make_error("Can not allocate memory for graphdef");
  }

  size_t readed;
  if (!file.Read(data, file_size, &readed) || readed != file_size) {
    file.Close();
    return common::make_error("Unable to read file");
  }

  file.Close();

  TF_Buffer* buf = TF_NewBuffer();
  buf->data = data;
  buf->length = file_size;
  buf->data_deallocator = free_buffer;
  return Load(buf);
}

common::Error Model::Start() {
  if (!graph_) {
    return common::make_error("Invalid graph NULL");
  }

  if (input_layer_name_.empty()) {
    return common::make_error("Input layer name has not been set");
  }

  if (output_layer_name_.empty()) {
    return common::make_error("Output layer name has not been set");
  }

  TF_Operation* in_operation = TF_GraphOperationByName(graph_, input_layer_name_.c_str());
  if (!in_operation) {
    return common::make_error("No valid input node provided");
  }

  TF_Operation* out_operation = TF_GraphOperationByName(graph_, output_layer_name_.c_str());
  if (!out_operation) {
    return common::make_error("No valid output node provided");
  }

  in_operation_ = in_operation;
  out_operation_ = out_operation;
  return common::Error();
}

std::string Model::GetInputLayerName() const {
  return input_layer_name_;
}

void Model::SetInputLayerName(const std::string& name) {
  input_layer_name_ = name;
}

std::string Model::GetOutputLayerName() const {
  return output_layer_name_;
}

void Model::SetOutputLayerName(const std::string& name) {
  output_layer_name_ = name;
}

TF_Graph* Model::GetGraph() const {
  return graph_;
}

TF_Operation* Model::GetOutputOperation() const {
  return out_operation_;
}

TF_Operation* Model::GetInputOperation() const {
  return in_operation_;
}

common::Error Model::Load(TF_Buffer* buffer) {
  if (!buffer) {
    TF_DeleteBuffer(buffer);
    return common::make_error_inval();
  }

  if (graph_) {
    TF_DeleteBuffer(buffer);
    return common::make_error_inval();
  }

  tf_status_locker_t pstatus(TF_NewStatus(), TF_DeleteStatus);
  if (!pstatus) {
    TF_DeleteBuffer(buffer);
    return common::make_error("Cannot allocate status");
  }

  tf_importgraphdefopt_locker_t pgopts(TF_NewImportGraphDefOptions(), TF_DeleteImportGraphDefOptions);
  if (!pgopts) {
    TF_DeleteBuffer(buffer);
    return common::make_error("Cannot allocate graph options");
  }

  TF_Status* status = pstatus.get();
  TF_Graph* graph = TF_NewGraph();
  TF_ImportGraphDefOptions* gopts = pgopts.get();
  TF_GraphImportGraphDef(graph, buffer, gopts, status);

  TF_Code code = TF_GetCode(status);
  if (code != TF_OK) {
    TF_DeleteBuffer(buffer);
    TF_DeleteGraph(graph);
    return common::make_error(TF_Message(status));
  }

  buffer_ = buffer;
  graph_ = graph;
  return common::Error();
}

Model::~Model() {
  if (graph_) {
    TF_DeleteGraph(graph_);
    graph_ = nullptr;
  }

  if (buffer_) {
    TF_DeleteBuffer(buffer_);
    buffer_ = nullptr;
  }
}

}  // namespace tensorflow
}  // namespace fastoml
