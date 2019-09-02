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

#include <map>

#include <common/file_system/file.h>

namespace {
std::string GetStringFromStatus (ncStatus_t status) {
  static std::map<int, const std::string> string_descriptions ({
    {NC_OK, "Everything OK"},
    {NC_BUSY, "Device is busy, retry later"},
    {NC_ERROR, "Error communicating with the device"},
    {NC_OUT_OF_MEMORY, "Out of memory"},
    {NC_DEVICE_NOT_FOUND, "No device at the given index or name"},
    {NC_INVALID_PARAMETERS, "At least one of the given parameters is wrong"},
    {NC_TIMEOUT, "Timeout in the communication with the device"},
    {NC_MVCMD_NOT_FOUND, "The file to boot Myriad was not found"},
    {NC_NOT_ALLOCATED, "The graph or device has been closed during the operation"},
    {NC_UNAUTHORIZED, "Unauthorized operation"},
    {NC_UNSUPPORTED_GRAPH_FILE, "The graph file version is not supported"},
    {NC_UNSUPPORTED_CONFIGURATION_FILE, "The configuration file version is not supported"},
    {NC_UNSUPPORTED_FEATURE, "Not supported by this FW version"},
    {NC_MYRIAD_ERROR, "An error has been reported by the device, use NC_DEVICE_DEBUG_INFO or NC_GRAPH_DEBUG_INFO"},
    {NC_INVALID_DATA_LENGTH, "invalid data length has been passed when get/set option"},
    {NC_INVALID_HANDLE, "handle to object that is invalid"}
  });

  auto search = string_descriptions.find (status);
  if (string_descriptions.end () == search) {
    return "Unable to find enum value, outdated list";
  }

  return search->second;
}
}

namespace fastoml {
namespace ncsdk {

Model::Model() : graph_(nullptr), graph_data_(nullptr), graph_size_(0), name_() {}

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
  if (name_.empty()) {
    return common::make_errno_error("Invalid graph name", EINVAL);
  }

  if (graph_) {
    return common::make_errno_error("Model already started", EINVAL);
  }

  ncGraphHandle_t* lgraph = nullptr;
  ncStatus_t ret = ncGraphCreate (name_.c_str(), &lgraph);
  if (NC_OK != ret) {
    return common::make_errno_error(GetStringFromStatus(ret), EINVAL);
  }

  graph_ = lgraph;
  return common::ErrnoError();
}

Model::~Model() {
  if (graph_) {
    ncStatus_t ret = ncGraphDestroy(&graph_);
    if (NC_OK != ret) {}
    graph_ = nullptr;
  }

  if (graph_data_) {
    free(graph_data_);
    graph_data_ = nullptr;
  }

  if (graph_size_) {
    graph_size_ = 0;
  }

  if(!name_.empty()) {
    name_.clear();;
  }
}

}  // namespace ncsdk
}  // namespace fastoml
