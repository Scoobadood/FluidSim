#ifndef FLUIDSIM_RENDERER_H_
#define FLUIDSIM_RENDERER_H_

#include <cstdint>
#include <GLHelpers/gl_common.h>
#include <common/common.h>
#include "particles/particle_system.h"

class Renderer {
 public:
  Renderer(uint32_t width, uint32_t height);

  void Render(const std::shared_ptr<ParticleSystem> &particle_system,
              const glm::mat4 &cam_rot);

  void SetFrameBufferSize(uint32_t width, uint32_t height);
  void InitShaders();

 private:
  void InitGL();

  GLuint vao_;
  GLuint vbo_;
  GLuint ebo_;
  glm::mat4 projection_;
  GLsizei frame_buffer_width_;
  GLsizei frame_buffer_height_;
  glm::vec3 camera_position_;
  std::shared_ptr<Shader> shader_;
};
#endif //FLUIDSIM_RENDERER_H_
