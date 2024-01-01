#include "spdlog/spdlog.h"

#include "main.h"

#include "scene_data.h"
#include "spdlog/cfg/env.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "gl_common.h"
#include "shader.h"

/*
 *     // Left face
    vertex_data.insert(vertex_data.end(), {min_x, max_y, max_z, -1, 0, 0});
    vertex_data.insert(vertex_data.end(), {min_x, max_y, min_z, -1, 0, 0});
    vertex_data.insert(vertex_data.end(), {min_x, min_y, min_z, -1, 0, 0});
    vertex_data.insert(vertex_data.end(), {min_x, min_y, max_z, -1, 0, 0});
    // Front face
    vertex_data.insert(vertex_data.end(), {min_x, max_y, min_z, 0, 0, -1});
    vertex_data.insert(vertex_data.end(), {max_x, max_y, min_z, 0, 0, -1});
    vertex_data.insert(vertex_data.end(), {max_x, min_y, min_z, 0, 0, -1});
    vertex_data.insert(vertex_data.end(), {min_x, min_y, min_z, 0, 0, -1});
    // Right face
    vertex_data.insert(vertex_data.end(), {max_x, max_y, min_z, 1, 0, 0});
    vertex_data.insert(vertex_data.end(), {max_x, max_y, max_z, 1, 0, 0});
    vertex_data.insert(vertex_data.end(), {max_x, min_y, max_z, 1, 0, 0});
    vertex_data.insert(vertex_data.end(), {max_x, min_y, min_z, 1, 0, 0});
    // Back face
    vertex_data.insert(vertex_data.end(), {max_x, max_y, max_z, 0, 0, 1});
    vertex_data.insert(vertex_data.end(), {min_x, max_y, max_z, 0, 0, 1});
    vertex_data.insert(vertex_data.end(), {min_x, min_y, max_z, 0, 0, 1});
    vertex_data.insert(vertex_data.end(), {max_x, min_y, max_z, 0, 0, 1});
    // Top face
    vertex_data.insert(vertex_data.end(), {min_x, max_y, max_z, 0, 1, 0});
    vertex_data.insert(vertex_data.end(), {max_x, max_y, max_z, 0, 1, 0});
    vertex_data.insert(vertex_data.end(), {max_x, max_y, min_z, 0, 1, 0});
    vertex_data.insert(vertex_data.end(), {min_x, max_y, min_z, 0, 1, 0});
    // Bottom face
    vertex_data.insert(vertex_data.end(), {min_x, min_y, min_z, 0, -1, 0});
    vertex_data.insert(vertex_data.end(), {max_x, min_y, min_z, 0, -1, 0});
    vertex_data.insert(vertex_data.end(), {max_x, min_y, max_z, 0, -1, 0});
    vertex_data.insert(vertex_data.end(), {min_x, min_y, max_z, 0, -1, 0});

 */

const int32_t POS_ATTR = 0;
const int32_t NORM_ATTR = 1;

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
 *  Generate the geometry for the scene.
 *  We will have N columns, each of which is WxD cross section and H[n] high
 *  The columns will be adjacent and centred on the origin.
 * ******************************************************************************************/
void generate_geometry(float width, float depth, const std::vector<float> &heights,
                       std::vector<float> &vertex_data,
                       std::vector<uint32_t> &indices) {
  auto total_width = (float) heights.size() * width;
  const auto min_y = 0.0f;
  const auto min_z = -(depth / 2.0f);
  const auto max_z = (depth / 2.0f);
  auto min_x = -(total_width / 2.0f);

  uint32_t base_indices[36] = {0, 1, 2,
                               0, 2, 3,
                               4, 5, 6,
                               4, 6, 7,
                               8, 9, 10,
                               8, 10, 11,
                               12, 13, 14,
                               12, 14, 15,
                               16, 17, 18,
                               16, 18, 19,
                               20, 21, 22,
                               20, 22, 23
  };

  auto base_vertex = 0;
  for (float max_y: heights) {
    auto max_x = min_x + width;
    // Left face
    vertex_data.insert(vertex_data.end(), {min_x, max_y, max_z, -1.0f, 0.0f, 0.0f});
    vertex_data.insert(vertex_data.end(), {min_x, max_y, min_z, -1.0f, 0.0f, 0.0f});
    vertex_data.insert(vertex_data.end(), {min_x, min_y, min_z, -1, 0, 0});
    vertex_data.insert(vertex_data.end(), {min_x, min_y, max_z, -1, 0, 0});
    // Front face
    vertex_data.insert(vertex_data.end(), {min_x, max_y, min_z, 0.0f, 0.0f, -1.0f});
    vertex_data.insert(vertex_data.end(), {max_x, max_y, min_z, 0.0f, 0.0f, -1.0f});
    vertex_data.insert(vertex_data.end(), {max_x, min_y, min_z, 0.0f, 0.0f, -1.0f});
    vertex_data.insert(vertex_data.end(), {min_x, min_y, min_z, 0.0f, 0.0f, -1.0f});
    // Right face
    vertex_data.insert(vertex_data.end(), {max_x, max_y, min_z, 1.0f, 0.0f, 0.0f});
    vertex_data.insert(vertex_data.end(), {max_x, max_y, max_z, 1.0f, 0.0f, 0.0f});
    vertex_data.insert(vertex_data.end(), {max_x, min_y, max_z, 1.0f, 0.0f, 0.0f});
    vertex_data.insert(vertex_data.end(), {max_x, min_y, min_z, 1.0f, 0.0f, 0.0f});
    // Back face
    vertex_data.insert(vertex_data.end(), {max_x, max_y, max_z, 0.0f, 0.0f, 1.0f});
    vertex_data.insert(vertex_data.end(), {min_x, max_y, max_z, 0.0f, 0.0f, 1.0f});
    vertex_data.insert(vertex_data.end(), {min_x, min_y, max_z, 0.0f, 0.0f, 1.0f});
    vertex_data.insert(vertex_data.end(), {max_x, min_y, max_z, 0.0f, 0.0f, 1.0f});
    // Top face
    vertex_data.insert(vertex_data.end(), {min_x, max_y, max_z, 0.0f, 1.0f, 0.0f});
    vertex_data.insert(vertex_data.end(), {max_x, max_y, max_z, 0.0f, 1.0f, 0.0f});
    vertex_data.insert(vertex_data.end(), {max_x, max_y, min_z, 0.0f, 1.0f, 0.0f});
    vertex_data.insert(vertex_data.end(), {min_x, max_y, min_z, 0.0f, 1.0f, 0.0f});
    // Bottom face
    vertex_data.insert(vertex_data.end(), {min_x, min_y, min_z, 0.0f, -1.0f, 0.0f});
    vertex_data.insert(vertex_data.end(), {max_x, min_y, min_z, 0.0f, -1.0f, 0.0f});
    vertex_data.insert(vertex_data.end(), {max_x, min_y, max_z, 0.0f, -1.0f, 0.0f});
    vertex_data.insert(vertex_data.end(), {min_x, min_y, max_z, 0.0f, -1.0f, 0.0f});


    for (unsigned int base_index: base_indices) {
      indices.push_back(base_index + base_vertex);
    }
    base_vertex += 24;

    min_x += width;
  }

}

/* ******************************************************************************************
 *  Create the geometry for the scene
 * ******************************************************************************************/
void create_geometry(std::vector<float> &col_heights, float col_width, float col_depth, GLuint &vao, GLuint &vbo,
                     GLuint &ebo, GLsizei &num_elements) {
  spdlog::info("Creating geometry");

  std::vector<float> vertex_data;
  std::vector<uint32_t> index_data;
  std::vector<float> heights;
  generate_geometry(col_width, col_depth, col_heights, vertex_data, index_data);

  // VAO
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  auto bytes_per_vertex = 4 * (3 + 3); // No textures yet
  num_elements = index_data.size();

  // Vertex locations
  glGenBuffers(1, &vbo);
  glGenBuffers(1, &ebo);

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, vertex_data.size() * sizeof(float), vertex_data.data(), GL_STATIC_DRAW);
  glEnableVertexAttribArray(POS_ATTR);
  glVertexAttribPointer(POS_ATTR, 3, GL_FLOAT, GL_FALSE, bytes_per_vertex, (GLvoid *)
          nullptr);

  glEnableVertexAttribArray(NORM_ATTR);
  glVertexAttribPointer(NORM_ATTR, 3, GL_FLOAT, GL_FALSE, bytes_per_vertex, (GLvoid *)
          12);

// Normals and textures
///    glEnableVertexAttribArray(tx_attr);
//    glVertexAttribPointer(tx_attr, 2, GL_FLOAT, GL_FALSE, vtx_sz, (GLvoid *) 24);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_data.size() * sizeof(uint32_t), index_data.data(), GL_STATIC_DRAW);

  CHECK_GL_ERROR("create geometry");
}

void reload_geometry(GLuint vao, GLuint vbo, std::vector<float> &new_heights, float col_width, float col_depth) {
  std::vector<float> vertex_data;
  std::vector<uint32_t> index_data;
  generate_geometry(col_width, col_depth, new_heights, vertex_data, index_data);


  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, vertex_data.size() * sizeof(float), vertex_data.data(), GL_STATIC_DRAW);

// Normals and textures
///    glEnableVertexAttribArray(tx_attr);
//    glVertexAttribPointer(tx_attr, 2, GL_FLOAT, GL_FALSE, vtx_sz, (GLvoid *) 24);

  CHECK_GL_ERROR("regenerate geometry");
}


/* ******************************************************************************************
 *  Height field management
 * ******************************************************************************************/
std::vector<float> init_height_field(uint32_t num_columns) {
  std::vector<float> heights;

  heights.reserve(num_columns);
  auto theta = -M_PI;
  auto d_theta = (2 * (float) M_PI) / (float)num_columns;
  for (auto i = 0; i < num_columns; ++i) {
    heights.push_back(1.1f + std::cosf(theta));
    theta += d_theta;
  }

  return heights;
}

void adjust_height_field(std::vector<float>& heights, std::vector<float>& v_height) {
  for (auto i = 0; i < v_height.size(); ++i) {
    auto l = (i>0) ? heights[i-1] : heights[i];
    auto r = (i<heights.size()-1) ? heights[i+1] : heights[i];

    v_height[i] += ((l + r) * 0.5f - heights[i]);
    v_height[i] *= 0.99f;
  }
  for( auto  i=0; i< heights.size(); ++i) {
    heights[i] += v_height[i];
  }
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

  auto heights = init_height_field(20);
  std::vector<float> v_height(20,0);
  auto col_width = 0.25f;
  auto col_depth = 0.25f;
  create_geometry(heights, col_width, col_depth, vao, vbo, ebo, num_elements);

  //
  // Create a shader
  //
  auto shader = Shader::from_files("/Users/dave/Projects/FluidSim/renderer/pool/shaders/lighting.vert",
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
    view = glm::translate(view, glm::vec3(0, -3, -15));
    view = glm::rotate(view, (float) M_PI_2, glm::vec3(0, 1, 0));
    glm::mat4 model = glm::mat4(1.0);
//  model = glm::translate(model,glm::vec3(0,0,-10));
    model = glm::rotate(model, (float)M_PI_2, glm::vec3(0, 1, 0));

    shader->use();
    shader->set_uniform("project", project);
    shader->set_uniform("view", view);
    shader->set_uniform("model", model);

    shader->set_uniform("light_dir", glm::vec3(0, -10, -10));
    shader->set_uniform("light_intensity", 1.0f);

    shader->set_uniform("kd", 0.8f);
    shader->set_uniform("ks", 0.5f);
    shader->set_uniform("ka", 0.1f);
    shader->set_uniform("alpha", 10.0f);
    shader->set_uniform("object_colour", glm::vec3{0.0f, 0.3f, 0.8f});


    glDrawElements(GL_TRIANGLES, num_elements, GL_UNSIGNED_INT, (void *) nullptr);
    CHECK_GL_ERROR("Render");

    //
    // Update geometry
    //
    adjust_height_field(heights, v_height);
    reload_geometry(vao, vbo, heights, col_width, col_depth);

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