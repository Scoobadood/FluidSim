#include "spdlog/spdlog.h"

#include "main.h"

#include "scene_data.h"
#include "spdlog/cfg/env.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "gl_common.h"
#include "shader.h"


const int32_t POS_ATTR = 0;

/* ******************************************************************************************
 *  Error macro
 * ******************************************************************************************/
GLenum glerr;
#define CHECK_GL_ERROR(txt)   \
if ((glerr = glGetError()) != GL_NO_ERROR){ \
spdlog::critical("GL Error {} : {:x}", txt, glerr); \
throw std::runtime_error("GLR"); \
} \


/* ******************************************************************************************
 *  Handlers for GLFW
 * ******************************************************************************************/

void idle_handler() {}

/* ******************************************************************************************
 *  Initialise OpenGL, GLEW and GLFW
 * ******************************************************************************************/
GLFWwindow *initialise() {
  if (!glfwInit()) {
    spdlog::critical("Failed to initialise GLFW");
    throw std::runtime_error("Failed to initialise GLFW");
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_DOUBLEBUFFER, GL_TRUE);
  GLFWwindow *window = glfwCreateWindow(800, 600,
                                        "Sim",
                                        nullptr, nullptr);
  if (!window) {
    glfwTerminate();
    spdlog::critical("Failed to create window");
    throw std::runtime_error("Failed to create window");
  }

  // Make current context live
  glfwMakeContextCurrent(window);

#ifndef __APPLE__
  // Need context before we do this.
  GLenum err = glewInit();
  if( GLEW_OK != err) {
    glfwTerminate();
    auto msg = fmt("Error: {}", (const char *)glewGetErrorString(err));
    spdlog::critical(msg);
    throw std::runtime_error(msg);
  }
#endif
  return window;
}

/* ******************************************************************************************
 *  Create the geometry for the scene
 * ******************************************************************************************/
void create_geometry(GLuint &vao, GLuint &vbo, GLuint &ebo, GLsizei &num_elements) {
  spdlog::info("Creating geometry");

  // VAO
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  auto vtx_sz = 12; // No normals or textures yet
  auto num_vertices = sizeof(CUBE_VERTEX_DATA) / 3;
  num_elements = sizeof(CUBE_INDICES);

  // Vertex locations
  glGenBuffers(1, &vbo);
  glGenBuffers(1, &ebo);

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, vtx_sz * num_vertices, CUBE_VERTEX_DATA, GL_STATIC_DRAW);
  glEnableVertexAttribArray(POS_ATTR);
  glVertexAttribPointer(POS_ATTR, 3, GL_FLOAT, GL_FALSE, vtx_sz, (GLvoid *)
          nullptr);

// Normals and textures
///    glEnableVertexAttribArray(tx_attr);
//    glVertexAttribPointer(tx_attr, 2, GL_FLOAT, GL_FALSE, vtx_sz, (GLvoid *) 24);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, num_elements, CUBE_INDICES, GL_STATIC_DRAW);

  CHECK_GL_ERROR("create geometry");
}

/*
 * Main
 */
int main(int argc, char *argv[]) {
  spdlog::cfg::load_env_levels();

  GLFWwindow *window;
  try {
    window = initialise();
  } catch (std::exception &e) {
    return EXIT_FAILURE;
  }

  glfwSwapInterval(1); // Enable vsync

  //
  // Create initial geometry
  //
  GLuint vao, vbo, ebo;
  GLsizei num_elements;
  create_geometry(vao, vbo, ebo, num_elements);

  //
  // Create a shader
  //
  auto shader =   Shader::from_files("/Users/dave/Projects/FluidSim/renderer/pool/shaders/lighting.vert",
                                   "/Users/dave/Projects/FluidSim/renderer/pool/shaders/lighting.frag");
  CHECK_GL_ERROR("Create shader");

  while (!glfwWindowShouldClose(window)) {
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    auto ratio = (float) width / (float) height;
    glViewport(0, 0, width, height);

    glm::mat4 project = glm::perspective(glm::radians(35.0f), ratio, 0.1f, 35.0f);


    //
    // Main render loop
    //
    glEnable(GL_DEPTH_TEST);

    glClearColor(0.3, 0.3, 0.3, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Bind VAO
    glBindVertexArray(vao);

    // Update view
    glm::mat4 view{1};
    view = glm::translate(view, glm::vec3(0, 3, -15));
    view = glm::rotate(view, (float) M_PI_2, glm::vec3(0, 1, 0));

    shader->use();
    shader->set_uniform("project", project);
    shader->set_uniform("view", view);
    shader->set_uniform("light_color", glm::vec3{1, 1, 1});
    shader->set_uniform("object_color", glm::vec3{0.7f, 0.6f, 0.5f});

    glm::mat4 model = glm::mat4(1.0);
//  model = glm::translate(model,glm::vec3(0,0,-10));
//  model = glm::rotate(model, head_angle_[i], glm::vec3(0, 1, 0));
    shader->set_uniform("model", model);
//  shader_->set_uniform("alpha", head_alpha_[i]);

    glDrawElements(GL_TRIANGLES, num_elements, GL_UNSIGNED_INT, (void *) nullptr);
    CHECK_GL_ERROR("Render");

    //
    // End of frame
    //
    idle_handler();
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;

}