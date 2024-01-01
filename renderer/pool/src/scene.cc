#include "scene.h"
#include "spdlog/spdlog.h"
#include "gl_common.h"
#include "shader.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"

const int32_t POS_ATTR = 0;

GLenum glerr;
#define CHECK_GL_ERROR(txt)   \
if ((glerr = glGetError()) != GL_NO_ERROR){ \
spdlog::critical("GL Error {} : {:x}", txt, glerr); \
throw std::runtime_error("GLR"); \
} \

/**
 * Create VAO, VBO and EBO for a simple cube.
 * @param vao
 * @param vbo
 * @param ebo
 * @param num_elements
 */
void CreateGeometry(GLuint &vao, GLuint &vbo, GLuint &ebo, GLsizei &num_elements) {
  spdlog::info("Creating geometry");

  // VAO
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  // Vertex size is 3 (position)
  /*
   * 0+-----------+4
   *  |\          :\
   *  | \         : \
   *  | 1+--------:--+5
   *  |  |        :  |
   * 3+..|........:7 |
   *   \ |           |
   *    \|           |
   *    2+-----------+6
   *
   */
  float vertex_data[] = {
          -1, 1, 1,
          -1, 1, -1,
          -1, -1, -1,
          -1, -1, 1,
          1, 1, 1,
          1, 1, -1,
          1, -1, -1,
          1, -1, 1,
  };
  uint32_t indices[] = {  // note that we start from 0!
          0, 1, 2,
          0, 2, 3,
          1, 5,6,
          1,6,2,
          5,4,7,
          5,7,6,
          4,0,3,
          4,3,7,
          0,4,5,
          0,5,1,
          2,6,7,
          2,7,3
  };
  auto vtx_sz = (3) * 4;
  auto num_vertices = 8;
  num_elements = 36;

  // Vertex locations
  glGenBuffers(1, &vbo);
  glGenBuffers(1, &ebo);

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, vtx_sz * num_vertices, vertex_data, GL_STATIC_DRAW);
  glEnableVertexAttribArray(POS_ATTR);
  glVertexAttribPointer(POS_ATTR, 3, GL_FLOAT, GL_FALSE, vtx_sz, (GLvoid *)
          nullptr);

  // Normals and textures
///    glEnableVertexAttribArray(tx_attr);
//    glVertexAttribPointer(tx_attr, 2, GL_FLOAT, GL_FALSE, vtx_sz, (GLvoid *) 24);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

  CHECK_GL_ERROR("create geometry");
}

/*
 * Create the geometry for a pool
 */
Scene::Scene()//
        : vao_{0} //
        , vbo_{0} //
        , ebo_{0} //
        , num_elements_{0} //
{
  CreateGeometry(vao_, vbo_, ebo_, num_elements_);
  shader_ = Shader::from_files("/Users/dave/Projects/FluidSim/renderer/pool/shaders/lighting.vert",
                               "/Users/dave/Projects/FluidSim/renderer/pool/shaders/lighting.frag");
  CHECK_GL_ERROR("Create shader");

  // Set up the projection matrix and bind it.
  float fov = 35.0f;
  glm::mat4 project = glm::perspective(glm::radians(fov), 1.0f, 0.1f, 35.0f);

  // Initialise spot attributes to be overhead
  shader_->use();
  shader_->set_uniform("project", project);
}

void Scene::Render() {
  glEnable(GL_DEPTH_TEST);
  glClearColor(0.3, 0.3, 0.3, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Bind VAO
  glBindVertexArray(vao_);

  // Update view
  glm::mat4 view{1};
  view = glm::translate(view, glm::vec3(0, 3, -15));
  view = glm::rotate(view, (float) M_PI_2, glm::vec3(0, 1, 0));

  shader_->use();
  shader_->set_uniform("view", view);
  shader_->set_uniform("light_color", glm::vec3{1, 1, 1});
  shader_->set_uniform("object_color", glm::vec3{0.7f, 0.6f, 0.5f});

  glm::mat4 model = glm::mat4(1.0);
//  model = glm::translate(model,glm::vec3(0,0,-10));
//  model = glm::rotate(model, head_angle_[i], glm::vec3(0, 1, 0));
  shader_->set_uniform("model", model);
//  shader_->set_uniform("alpha", head_alpha_[i]);

  glDrawElements(GL_TRIANGLES, num_elements_, GL_UNSIGNED_INT, (void *) nullptr);
  CHECK_GL_ERROR("Render");
}