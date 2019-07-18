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

#include <gtest/gtest.h>

#include <fastoml/backend.h>

#define PB_YOLOV2_FILEPATH PROJECT_TEST_SOURCES_DIR "/data/graph_tinyyolov2_tensorflow.pb"

TEST(Tensor, Create) {
  fastoml::BackEnd* back = nullptr;
  common::Error err = fastoml::BackEnd::MakeBackEnd(fastoml::TENSORFLOW, &back);
  ASSERT_FALSE(err);
  ASSERT_TRUE(back);

  fastoml::BackendMeta meta = back->GetMeta();
  ASSERT_EQ(fastoml::TENSORFLOW, meta.code);
  err = back->LoadGraph(common::file_system::ascii_file_string_path(PB_YOLOV2_FILEPATH));
  ASSERT_FALSE(err);

  err = back->Start();
  ASSERT_FALSE(err);

  err = back->Stop();
  ASSERT_FALSE(err);
  delete back;
}
