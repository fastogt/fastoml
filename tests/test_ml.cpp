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

#include <gtest/gtest.h>

#include <fastoml/backend.h>
#include <fastoml/tensorflow/parameters.h>

#define DIM 224

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image_resize.h"

#define PB_INCEPTIONV2_FILEPATH PROJECT_TEST_SOURCES_DIR "/data/graph_inceptionv2_tensorflow.pb"
#define IMAGE_FILEPATH PROJECT_TEST_SOURCES_DIR "/data/test.png"

size_t GetTopPrediction(fastoml::IPrediction* prediction) {
  size_t index = 0;
  float max = -1;
  size_t num_labels = prediction->GetResultSize() / sizeof(float);

  for (size_t i = 0; i < num_labels; ++i) {
    float current = 0;
    common::Error err = prediction->At(i, &current);
    if (err) {
      break;
    }

    if (current > max) {
      max = current;
      index = i;
    }
  }

  return index;
}

std::unique_ptr<float[]> PreProcessImage(const unsigned char* input,
                                         int width,
                                         int height,
                                         int reqwidth,
                                         int reqheight) {
  const int channels = 3;
  const int scaled_size = channels * reqwidth * reqheight;

  std::unique_ptr<unsigned char[]> scaled(new unsigned char[scaled_size]);
  std::unique_ptr<float[]> adjusted(new float[scaled_size]);

  stbir_resize_uint8(input, width, height, 0, scaled.get(), reqwidth, reqheight, 0, channels);

  for (int i = 0; i < scaled_size; i += channels) {
    adjusted[i + 0] = static_cast<float>(scaled[i + 0]) / 255;
    adjusted[i + 1] = static_cast<float>(scaled[i + 1]) / 255;
    adjusted[i + 2] = static_cast<float>(scaled[i + 2]) / 255;
  }

  return adjusted;
}

std::unique_ptr<float[]> LoadImage(const std::string& path, int reqwidth, int reqheight, int* width, int* height) {
  int channels = 3;
  int cp;

  unsigned char* img = stbi_load(path.c_str(), width, height, &cp, channels);
  if (!img) {
    std::cerr << "The picture " << path << " could not be loaded";
    return nullptr;
  }

  auto ret = PreProcessImage(img, *width, *height, reqwidth, reqheight);
  free(img);

  return ret;
}

TEST(Tensor, Create) {
  fastoml::BackEnd* back = nullptr;
  common::Error err = fastoml::BackEnd::MakeBackEnd(fastoml::TENSORFLOW, &back);
  ASSERT_FALSE(err);
  ASSERT_TRUE(back);

  fastoml::BackendMeta meta = back->GetMeta();
  ASSERT_EQ(fastoml::TENSORFLOW, meta.code);
  err = back->LoadGraph(common::file_system::ascii_file_string_path(PB_INCEPTIONV2_FILEPATH));
  ASSERT_FALSE(err);

  common::Value* input_layer_value = common::Value::CreateStringValueFromBasicString("input");
  err = back->SetProperty(INPUT_LAYER_PROPERTY, input_layer_value);
  ASSERT_FALSE(err);
  delete input_layer_value;

  common::Value* output_layer_value = common::Value::CreateStringValueFromBasicString("Softmax");
  err = back->SetProperty(OUTPUT_LAYER_PROPERTY, output_layer_value);
  ASSERT_FALSE(err);
  delete output_layer_value;

  int width, height;
  std::unique_ptr<float[]> image_data = LoadImage(IMAGE_FILEPATH, DIM, DIM, &width, &height);
  ASSERT_TRUE(image_data);

  fastoml::IFrame* frame = nullptr;
  err = back->MakeFrame(common::draw::Size(DIM, DIM), fastoml::ImageFormat::RGB, image_data.get(), &frame);
  ASSERT_FALSE(err);
  ASSERT_TRUE(frame);

  err = back->Start();
  ASSERT_FALSE(err);

  fastoml::IPrediction* predict = nullptr;
  err = back->Predict(frame, &predict);
  ASSERT_FALSE(err);
  ASSERT_TRUE(predict);

  size_t top_pred = GetTopPrediction(predict);
  ASSERT_EQ(top_pred, 252);

  err = back->Stop();
  ASSERT_FALSE(err);

  delete predict;
  delete frame;
  delete back;
}
