/*  Copyright (C) 2014-2020 FastoGT. All right reserved.

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

#include <common/draw/size.h>
#include <common/error.h>

#include <fastoml/types.h>

namespace fastoml {

class IFrame {
 public:
  typedef void* data_t;

  common::draw::Size GetSize() const;
  ImageFormat GetFormat() const;
  data_t GetData() const;

  virtual ~IFrame();

 protected:
  IFrame(const common::draw::Size& size, ImageFormat::Type format, data_t data);

 private:
  common::draw::Size size_;
  ImageFormat format_;
  data_t data_;
};

}  // namespace fastoml
