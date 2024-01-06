#ifndef GL_HELPERS_TEXTURE_H
#define GL_HELPERS_TEXTURE_H

#include <string>

#define STB_IMAGE_IMPLEMENTATION

#include "stb_image.h"

class Texture {
public:
  Texture(const std::string &file_name);

  ~Texture();

  void BindToTextureUnit(uint32_t tu);

  static std::shared_ptr<Texture> FromImageFile(const std::string &file_name);

private:
  Texture();

  void SetImage(uint8_t *image_data, int32_t width, int32_t height, int32_t channels);

  uint32_t texture_id_;
  static int32_t MAX_TEXTURE_UNIT_ID;
};

#endif
