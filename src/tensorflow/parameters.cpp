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

#include <fastoml/tensorflow/parameters.h>

namespace fastoml {
namespace tensorflow {

const std::vector<ParameterMeta> kParameters = {
    {VERSION_PROPERTY, "Tensorflow version", ParameterMeta::Flags::READ, common::Value::TYPE_STRING},
    {INPUT_LAYER_PROPERTY, "Name of the input layer in the graph",
     ParameterMeta::Flags::READWRITE | ParameterMeta::Flags::WRITE_BEFORE_START, common::Value::TYPE_STRING},
    {OUTPUT_LAYER_PROPERTY, "Name of the output layer in the graph",
     ParameterMeta::Flags::READWRITE | ParameterMeta::Flags::WRITE_BEFORE_START, common::Value::TYPE_STRING}};

}  // namespace tensorflow
}  // namespace fastoml
