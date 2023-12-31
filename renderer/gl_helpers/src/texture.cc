#include "texture.h"
#include "spdlog/spdlog.h"
#include "gl_common.h"


int32_t Texture::MAX_TEXTURE_UNIT_ID = 0;

uint8_t *
LoadAndFlipImage(const std::string &file_name, int32_t &width, int32_t &height, int32_t &channels) {
  uint8_t *image_data = stbi_load(file_name.c_str(), &width, &height, &channels, 3);

  if (!image_data) {
    spdlog::error("Failed to load image file: {}", file_name);
    return nullptr;
  }

  // Flip image
  for (auto row = 0; row < height / 2; ++row) {
    for (auto col = 0; col < width; ++col) {
      auto from_idx = (row * width + col) * 3;
      auto to_idx = ((height - row - 1) * width + col) * 3;
      for (auto c = 0; c < 3; ++c) {
        uint8_t t = image_data[from_idx + c];
        image_data[from_idx + c] = image_data[to_idx + c];
        image_data[to_idx + c] = t;
      }
    }
  }
  return image_data;
}

Texture::Texture(const std::string &file_name) {
  if (MAX_TEXTURE_UNIT_ID == 0) {
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS,
                  &MAX_TEXTURE_UNIT_ID);
  }

  glActiveTexture(GL_TEXTURE0);
  glGenTextures(1, &texture_id_);

  int32_t width, height, channels;
  auto image_data = LoadAndFlipImage(file_name, width, height, channels);
  if (!image_data) {
    throw std::runtime_error(fmt::format("Failed to load texture from image {}", file_name));
  }

  SetImage(image_data, width, height, channels);

  stbi_image_free(image_data);
}

Texture::~Texture() {
  if (texture_id_ != 0) glDeleteTextures(1, &texture_id_);
}

void
Texture::BindToTextureUnit(uint32_t tu) {
  if (tu >= MAX_TEXTURE_UNIT_ID) {
    spdlog::error("Invalid texture unit : {}, maximum allowed is {}",
                  tu, MAX_TEXTURE_UNIT_ID - 1);
    return;
  }

  glBindTexture(GL_TEXTURE0 + tu, texture_id_);
}

void
Texture::SetImage(uint8_t *image_data, int32_t width, int32_t height, int32_t channels) {
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture_id_);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);

  // load file to texture
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
               GL_RGB, GL_UNSIGNED_BYTE, image_data);
}
