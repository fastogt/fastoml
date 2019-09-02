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

#include <fastoml/ncsdk/types.h>

#include <mvnc.h>

namespace fastoml {
namespace ncsdk {

std::string GetStringFromStatus (int status) {
  if (status == NC_OK) {
    return "Everything OK";
  } else if (status == NC_BUSY) {
    return "Device is busy, retry later";
  } else if (status == NC_ERROR) {
    return "Error communicating with the device";
  } else if (status == NC_OUT_OF_MEMORY) {
    return "Out of memory";
  } else if (status == NC_DEVICE_NOT_FOUND) {
    return "No device at the given index or name";
  } else if (status == NC_INVALID_PARAMETERS) {
    return "At least one of the given parameters is wrong";
  } else if (status == NC_TIMEOUT) {
    return "Timeout in the communication with the device";
  } else if (status == NC_MVCMD_NOT_FOUND) {
    return "The file to boot Myriad was not found";
  } else if (status == NC_NOT_ALLOCATED) {
    return "The graph or device has been closed during the operation";
  } else if (status == NC_UNAUTHORIZED) {
    return "Unauthorized operation";
  } else if (status == NC_UNSUPPORTED_GRAPH_FILE) {
    return "The graph file version is not supported";
  } else if (status == NC_UNSUPPORTED_CONFIGURATION_FILE) {
    return "The configuration file version is not supported";
  } else if (status == NC_UNSUPPORTED_FEATURE) {
    return "Not supported by this FW version";
  } else if (status == NC_MYRIAD_ERROR) {
    return "An error has been reported by the device, use NC_DEVICE_DEBUG_INFO or NC_GRAPH_DEBUG_INFO";
  } else if (status == NC_INVALID_DATA_LENGTH) {
    return "Invalid data length has been passed when get/set option";
  } else if (status == NC_INVALID_HANDLE) {
    return "Handle to object that is invalid";
  }

  return "Unable to find enum value, outdated list";
}

}  // namespace ncsdk
}  // namespace fastoml
