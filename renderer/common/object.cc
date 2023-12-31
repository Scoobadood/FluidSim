#include "object.h"
#include "spdlog/spdlog-inl.h"
#include "mesh.h"
#include "texture.h"

#include "gl_common.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"

#include <vector>

GLenum glerr;
#define CHECK_GL_ERROR(txt)   \
if ((glerr = glGetError()) != GL_NO_ERROR){ \
spdlog::critical("GL Error {} : {:x}", txt, glerr); \
throw std::runtime_error("GLR"); \
} \


glm::mat4 view_for_dir(Object::ViewDir view_dir, float view_dist) {
  auto v = glm::mat4(1);
  switch (view_dir) {
    case Object::POS_X:
      v = glm::translate(v, glm::vec3(0, 0, -view_dist));
      v = glm::rotate(v, (float) M_PI_2, glm::vec3(0, 1, 0));
      break;
    case Object::POS_Y:
      v = glm::translate(v, glm::vec3(0, 0, -view_dist));
      v = glm::rotate(v, (float) M_PI_2, glm::vec3(1, 0, 0));
      break;
    case Object::POS_Z:
      v = glm::translate(v, glm::vec3(0, 0, -view_dist));
      v = glm::rotate(v, (float) M_PI, glm::vec3(0, 1, 0));
      break;
    case Object::NEG_X:
      v = glm::translate(v, glm::vec3(0, 0, -view_dist));
      v = glm::rotate(v, -(float) M_PI_2, glm::vec3(0, 1, 0));
      break;
    case Object::NEG_Y:
      v = glm::translate(v, glm::vec3(0, 0, -view_dist));
      v = glm::rotate(v, -(float) M_PI_2, glm::vec3(1, 0, 0));
      break;
    case Object::NEG_Z:
      v = glm::translate(v, glm::vec3(0, 0, -view_dist));
      break;
  }
  return v;
}

Object::Object(const std::string &file_name,
               bool include_normals,
               bool include_tex_coords) //
        : vao_{0} //
        , vbo_{0} //
        , ebo_{0} //
        , num_elements_{0} //
{
  init_shader();
  if (!shader_->is_good()) {
    return;
  }

  textured_shader_ = Shader::from_files("lighting.vert", "lighting_fourspot_tex.frag");
  if (!textured_shader_->is_good()) {
    return;
  }

  texture_ = std::make_shared<Texture>("african_head_diffuse.tga");

  CHECK_GL_ERROR("shader build")

  shader_->use();

  auto pos_attr = shader_->get_attribute_location("pos");
  if (pos_attr == -1) {
    spdlog::error("Invalid attribute location pos:{}", pos_attr);
    return;
  }

  uint32_t norm_attr = -1;
  if (include_normals) {
    norm_attr = shader_->get_attribute_location("normal");
    if (norm_attr == -1) {
      spdlog::warn("Missing attribute location norm. Disabling normals.");
      include_normals = false;
    }
  }

  uint32_t tx_attr = -1;
  if (include_tex_coords) {
    tx_attr = shader_->get_attribute_location("tex_coords");
    if (tx_attr == -1) {
      spdlog::warn("Missing attribute location tex_coords. Disabling textures.");
      include_tex_coords = false;
    }
  }

  load_obj(file_name, vao_, vbo_, ebo_,
           num_elements_, pos_attr,
           include_normals, norm_attr,
           include_tex_coords, tx_attr);
  CHECK_GL_ERROR("load obj")


  /*
   * Set up matrices
   */
  float fov = 20.0f;
  glm::mat4 project = glm::perspective(glm::radians(fov), 1.0f, 0.1f, 35.0f);

  // Initialise spot attributes to be overhead
  for (int i = 0; i < NUM_HEADS; i++) {
    spot_light_pos_[i] = head_position_[i] + glm::vec3(0, 3.5, 0);
    spot_light_dir_[i] = glm::vec3{0, -1, 0};
    spot_light_pos_[i + 4] = (head_position_[i] + head_position_[i]+ head_position_[i]);
    spot_light_dir_[i + 4] = -head_position_[i];
    spot_on_[i] = spot_on_[i+4] = 0.0f;
  }

  shader_->use();
  shader_->set_uniform("project", project);
  shader_->set_uniform("light_dir", glm::vec3(0, -1, -0.5));
  shader_->set_uniform("spot_light_angle", (float) (45.0f * M_PI / 180.0f));

  textured_shader_->use();
  textured_shader_->set_uniform("project", project);
  textured_shader_->set_uniform("light_dir", glm::vec3(0, -1, -0.5));
  textured_shader_->set_uniform("spot_light_angle", (float) (45.0f * M_PI / 180.0f));
}

Object::~Object() {
  destroy_buffers();
}

void Object::main_loop() {
  glEnable(GL_DEPTH_TEST);
  glClearColor(0.0, 0.0, 0.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glBindVertexArray(vao_);

  // Update view
  glm::mat4 view = view_for_dir(view_from_, view_dist_);

  // Set spotlight cam positions
  glm::vec3 slp[NUM_SPOTS];
  glm::vec3 sld[NUM_SPOTS];
  glm::mat3 dv = transpose(inverse(view));
  for (auto i = 0; i < NUM_SPOTS; ++i) {
    // Implicit vec3 construction here
    slp[i] = view * glm::vec4(spot_light_pos_[i], 1);
    sld[i] = dv * spot_light_dir_[i];
  }


  shader_->use();
  shader_->set_uniform("view", view);
  shader_->set_uniform("light_int", big_light_on_ ? 1.0f : 0.1f);
  shader_->set_uniform("spot_light_int", NUM_SPOTS, spot_on_);
  shader_->set_uniform("spot_light_pos", NUM_SPOTS, slp);
  shader_->set_uniform("spot_light_dir", NUM_SPOTS, sld);

  for (auto i = 0; i < NUM_HEADS - 1; ++i) {
    head_position_[i].x = M_SQRT2 * cos(rot_angle_ + (i * M_PI_2));
    head_position_[i].z = M_SQRT2 * sin(rot_angle_ + (i * M_PI_2));
    glm::mat4 model = glm::mat4(1.0);
    model = glm::translate(model, head_position_[i]);
    model = glm::rotate(model, head_angle_[i], glm::vec3(0, 1, 0));
    shader_->set_uniform("model", model);
    shader_->set_uniform("kd", 0.8f);
    shader_->set_uniform("ks", 0.5f);
    shader_->set_uniform("ka", 0.1f);
    shader_->set_uniform("colour", head_colour_[i]);
    shader_->set_uniform("alpha", head_alpha_[i]);
    glDrawElements(GL_TRIANGLES, num_elements_, GL_UNSIGNED_INT, (void *) nullptr);
    head_angle_[i] += 0.01f;
  }

  // Head 4
  head_position_[3].x = M_SQRT2 * cos(rot_angle_ + (3 * M_PI_2));
  head_position_[3].z = M_SQRT2 * sin(rot_angle_ + (3 * M_PI_2));
  glm::mat4 model = glm::mat4(1.0);
  model = glm::translate(model, head_position_[3]);
  model = glm::rotate(model, head_angle_[3], glm::vec3(0, 1, 0));

  texture_->BindToTextureUnit(0);

  textured_shader_->use();
  textured_shader_->set_uniform("view", view);
  textured_shader_->set_uniform("light_int", big_light_on_ ? 1.0f : 0.1f);
  textured_shader_->set_uniform("spot_light_int", NUM_SPOTS, spot_on_);
  textured_shader_->set_uniform("spot_light_pos", NUM_SPOTS, slp);
  textured_shader_->set_uniform("spot_light_dir", NUM_SPOTS, sld);

  textured_shader_->set_uniform("model", model);
  textured_shader_->set_uniform("kd", 0.8f);
  textured_shader_->set_uniform("ks", 0.f);
  textured_shader_->set_uniform("ka", 0.1f);
  textured_shader_->set_uniform("alpha", head_alpha_[3]);
  textured_shader_->set_uniform("head_texture", 0);
  glDrawElements(GL_TRIANGLES, num_elements_, GL_UNSIGNED_INT, (void *) nullptr);
  head_angle_[3] += 0.01f;

  rot_angle_ += 0.004f;
}

void
Object::init_shader() {
  shader_ = Shader::from_files("lighting.vert", "lighting.frag");
}

void
Object::destroy_buffers() {
  glDeleteBuffers(1, &vbo_);
  glDeleteBuffers(1, &ebo_);
  glDeleteBuffers(1, &vao_);
}
