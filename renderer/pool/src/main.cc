#include "spdlog/spdlog.h"

#include "main.h"

#include "spdlog/cfg/env.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "gl_common.h"
#include "shader.h"
#include "glm/detail/type_quat.hpp"
#include "glm/gtc/quaternion.hpp"
#include "trackball.h"

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
 *
 *  Handlers for GLFW
 *
 * ******************************************************************************************/

glm::vec<3, double> g_drag_start_p;
bool g_mouse_dragging = false;
glm::mat4 g_model_rot(1);
glm::dquat g_start_quat;
Trackball g_trackball{200.0f};

/*
 * Given a point on the screen, map it onto the sphere which projects
 * onto the screen.
 */
glm::vec<3, double> point_to_sphere(GLFWwindow *window, double xpos, double ypos) {
  int width, height;
  glfwGetWindowSize(window, &width, &height);

  float half_height = (float) height / 2.0f;
  float half_width = (float) width / 2.0f;
  float radius = std::fminf(half_height, half_width);
  auto px = (xpos - half_width) / radius;
  auto py = (half_height - ypos) / radius;
  double pz = 0.0;
  auto r2 = px * px + py * py;
  if (r2 > 1) {
    auto scale = 1.0 / std::sqrt(r2);
    px *= scale;
    py *= scale;
  } else {
    pz = std::sqrt(1.0 - r2);
  }
  return glm::normalize(glm::vec3(px, py, pz));
}


void mouse_button_callback(GLFWwindow *window, int button, int action, __attribute__((unused)) int mods) {
  if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
    g_mouse_dragging = true;
    double mx, my;
    glfwGetCursorPos(window, &mx, &my);

    g_drag_start_p = point_to_sphere(window, mx, my);
    g_start_quat = glm::quat_cast(g_model_rot);

  } else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {
    g_mouse_dragging = false;
  }
}

void mouse_move_callback(GLFWwindow *window, double xpos, double ypos) {
  if (!g_mouse_dragging) return;

  auto p = point_to_sphere(window, xpos, ypos);
  auto q_xyz = glm::cross(g_drag_start_p, p);
  auto q_w = glm::dot(g_drag_start_p, p);

  double s = std::sqrt((1.0f + q_w) * 2.0f);
  glm::dquat q_xform(0.5 * s, q_xyz / s);

  auto q_now = g_start_quat * q_xform;
  q_now /= length(q_now);

  g_model_rot = glm::mat4_cast(q_now);
}


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
 *  We will have N columns, each of which is WxD cross-section and H[n] high
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
  num_elements = (GLsizei) index_data.size();

  // Vertex locations
  glGenBuffers(1, &vbo);
  glGenBuffers(1, &ebo);

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr) (vertex_data.size() * sizeof(float)), vertex_data.data(), GL_STATIC_DRAW);
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
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr) (index_data.size() * sizeof(uint32_t)), index_data.data(),
               GL_STATIC_DRAW);

  CHECK_GL_ERROR("create geometry")
}

void reload_geometry(GLuint vao, GLuint vbo, std::vector<float> &new_heights, float col_width, float col_depth) {
  std::vector<float> vertex_data;
  std::vector<uint32_t> index_data;
  generate_geometry(col_width, col_depth, new_heights, vertex_data, index_data);


  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr) (vertex_data.size() * sizeof(float)), vertex_data.data(), GL_STATIC_DRAW);

// Normals and textures
///    glEnableVertexAttribArray(tx_attr);
//    glVertexAttribPointer(tx_attr, 2, GL_FLOAT, GL_FALSE, vtx_sz, (GLvoid *) 24);

  CHECK_GL_ERROR("regenerate geometry")
}


/* ******************************************************************************************
 *  Height field management
 * ******************************************************************************************/
std::vector<float> init_height_field(uint32_t num_columns) {
  std::vector<float> heights;

  heights.reserve(num_columns);
  auto theta = -(float) M_PI;
  auto d_theta = (2 * (float) M_PI) / (float) num_columns;
  for (auto i = 0; i < num_columns; ++i) {
    heights.push_back(1.1f + std::cosf(theta));
    theta += d_theta;
  }

  return heights;
}

void adjust_height_field(std::vector<float> &heights, std::vector<float> &v_height) {
  for (auto i = 0; i < v_height.size(); ++i) {
    auto l = (i > 0) ? heights[i - 1] : heights[i];
    auto r = (i < heights.size() - 1) ? heights[i + 1] : heights[i];

    v_height[i] += ((l + r) * 0.5f - heights[i]);
    v_height[i] *= 0.99f;
  }
  for (auto i = 0; i < heights.size(); ++i) {
    heights[i] += v_height[i];
  }
}

/* ******************************************************************************************
 *
 *  Main
 *
 * ******************************************************************************************/
int main(__attribute__((unused)) int argc, __attribute__((unused)) char *argv[]) {
  spdlog::cfg::load_env_levels();

  GLFWwindow *window;
  try {
    window = initialise();
  } catch (std::exception &e) {
    return EXIT_FAILURE;
  }

  glfwSwapInterval(1); // Enable vsync

  //
  // Set up callbacks
  //
  glfwSetMouseButtonCallback(window, mouse_button_callback);
  glfwSetCursorPosCallback(window, mouse_move_callback);

  //
  // Create initial geometry
  //
  GLuint vao, vbo, ebo;
  GLsizei num_elements;

  auto heights = init_height_field(20);
  std::vector<float> v_height(20, 0);
  auto col_width = 0.25f;
  auto col_depth = 0.25f;
  create_geometry(heights, col_width, col_depth, vao, vbo, ebo, num_elements);

  //
  // Create a shader
  //
  auto shader = Shader::from_files("/Users/dave/Projects/FluidSim/renderer/pool/shaders/lighting.vert",
                                   "/Users/dave/Projects/FluidSim/renderer/pool/shaders/lighting.frag");
  CHECK_GL_ERROR("Create shader")

//  g_cam_rot = glm::rotate(g_cam_rot, (float) M_PI_2, glm::vec3(0, 1, 0));
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
    view = glm::translate(view, glm::vec3(0, 0, -15));

    glm::mat4 model = glm::mat4(1.0);
//  model = glm::translate(model,glm::vec3(0,0,-10));
    model = model * g_model_rot;

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
    CHECK_GL_ERROR("Render")

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