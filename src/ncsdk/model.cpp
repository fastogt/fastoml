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

#include <fastoml/ncsdk/model.h>

#include <mvnc.h>

#include <common/file_system/file.h>

#include <fastoml/ncsdk/types.h>

namespace {
const char kGraphName[] = "NCSDK";
}

namespace fastoml {
namespace ncsdk {

Model::Model() : graph_(nullptr), graph_data_(nullptr), graph_size_(0) {}

common::ErrnoError Model::Load(const common::file_system::ascii_file_string_path& path) {
  if (!path.IsValid()) {
    return common::make_errno_error_inval();
  }

  common::file_system::ANSIFile file;
  common::ErrnoError errn = file.Open(path, "rb");
  if (errn) {
    return errn;
  }

  size_t file_size;
  errn = file.GetSize(&file_size);
  if (errn) {
    file.Close();
    return errn;
  }

  common::byte_t* data = reinterpret_cast<common::byte_t*>(malloc(file_size));
  if (!data) {
    file.Close();
    return common::make_errno_error("Can not allocate memory for graphdef", ENOMEM);
  }

  size_t readed;
  if (!file.Read(data, file_size, &readed) || readed != file_size) {
    file.Close();
    return common::make_errno_error("Unable to read file", EINTR);
  }

  file.Close();

  graph_data_ = data;
  graph_size_ = file_size;
  return common::ErrnoError();
}

common::ErrnoError Model::Start() {
  if (graph_) {
    return common::ErrnoError();
  }

  ncGraphHandle_t* lgraph = nullptr;
  ncStatus_t ret = ncGraphCreate (kGraphName, &lgraph);
  if (ret != NC_OK) {
    return common::make_errno_error(GetStringFromStatus(ret), EINVAL);
  }

  graph_ = lgraph;
  return common::ErrnoError();
}

common::ErrnoError Model::Stop() {
  if (graph_) {
    ncStatus_t ret = ncGraphDestroy(&graph_);
    if (ret != NC_OK) {
      return common::make_errno_error(GetStringFromStatus(ret), EINVAL);
    }
    graph_ = nullptr;
  }

  return common::ErrnoError();
}

ncGraphHandle_t *Model::GetHandler() const {
  return graph_;
}

void* Model::GetData() const {
  return graph_data_;
}

size_t Model::GetDataSize() const {
  return graph_size_;
}

Model::~Model() {
  if (graph_data_) {
    free(graph_data_);
    graph_data_ = nullptr;
  }

  if (graph_size_) {
    graph_size_ = 0;
  }
}

}  // namespace ncsdk
}  // namespace fastoml
