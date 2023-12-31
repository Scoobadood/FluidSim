#ifndef UTAH_ICG_OBJECT_H
#define UTAH_ICG_OBJECT_H

#include <vector>
#include "shader.h"

#ifdef __APPLE__

#include "OpenGL/gl3.h"
#include "texture.h"
#include "spdlog/spdlog.h"

#else
#include "GL/glew.h"
#include "GL/glext.h"
#include "GL/gl.h"
#endif

#include <memory>


class Object {
public:
  Object(const std::string &file_name,
         bool include_normals,
         bool include_tex_coords);

  ~Object();

  enum ViewDir {
    POS_X, POS_Y, POS_Z,
    NEG_X, NEG_Y, NEG_Z
  };

  void main_loop();

  void toggle_spot(int32_t spot_id) {
    if (spot_id < 0 || spot_id >= NUM_SPOTS) return;
    spot_on_[spot_id] = 1.0f - spot_on_[spot_id];
    spdlog::info("Spot {} {}", spot_id, spot_on_[spot_id] ? "on" : "off");
  }

  void big_light_on(bool on_off) { big_light_on_ = on_off; }

  void set_view_dir(ViewDir view_dir) { view_from_ = view_dir; }

  void set_view_dist(float view_dist) { view_dist_ = std::max(10.0f, std::min(30.0f, view_dist)); }
  float view_dist() { return view_dist_; }

private:
  void destroy_buffers();

  void init_shader();

  GLuint vao_;
  GLuint vbo_;
  GLuint ebo_;
  GLsizei num_elements_;
  std::shared_ptr<Texture> texture_;
  std::shared_ptr<Shader> shader_;
  std::shared_ptr<Shader> textured_shader_;

  const static int32_t NUM_HEADS = 4;
  float head_angle_[NUM_HEADS]{0.0f, 15.0f, 30.0f, 45.0f};
  ViewDir view_from_ = NEG_Z;
  float view_dist_ = 20.0f;
  bool big_light_on_ = true;
  float head_alpha_[NUM_HEADS]{100.0f, 1000.0f, 10.0f, 10.0f};
  glm::vec3 head_position_[NUM_HEADS]{{-1, 0, -1},
                                      {-1, 0, 1},
                                      {1,  0, 1},
                                      {1,  0, -1}};
  float rot_angle_ = 0.0f;
  glm::vec3 head_colour_[NUM_HEADS - 1]{{0.7f, 0.0f, 0.0f},
                                        {0.0f, 0.7f, 0.0f},
                                        {0.0f, 0.0f, 0.7f}
  };

  const static int32_t NUM_SPOTS = 8;
  glm::vec3 spot_light_pos_[NUM_SPOTS];
  glm::vec3 spot_light_dir_[NUM_SPOTS];
  float spot_on_[NUM_SPOTS];

};

#endif //UTAH_ICG_OBJECT_H
