#include "spdlog/cfg/env.h"
#include "spdlog/spdlog.h"

#include "main.h"
#include "gl_common.h"
#include "shader.h"
#include "texture.h"
#include "height_field_sim.h"
#include "geometry_helper.h"

#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/detail/type_quat.hpp"
#include "glm/gtc/quaternion.hpp"


const int32_t POS_ATTR = 0;
const int32_t NORM_ATTR = 1;
const int32_t COLR_ATTR = 2;
const int32_t TEXT_ATTR = 3;

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


void key_callback(GLFWwindow *window, int key,//
                  __attribute__((unused)) int scancode,
                  __attribute__((unused)) int action,//
                  __attribute__((unused)) int mods)//
{
  if (GLFW_KEY_R == key) {
    // Reset the simulation
    ((HeightField *) glfwGetWindowUserPointer(window))->Init(HeightField::PULSE);
  } else if (GLFW_KEY_W == key) {
    // Reset the simulation
    ((HeightField *) glfwGetWindowUserPointer(window))->Init(HeightField::WAVE);
  } else if (GLFW_KEY_S == key) {
    // Reset the simulation
    ((HeightField *) glfwGetWindowUserPointer(window))->Init(HeightField::CUBE);
  }
}

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
 *
 *  Create the geometry for the scene
 *
 * ******************************************************************************************/

void
create_geometry_buffers(const GeometryHelper::MetaData &s,//
                        GLuint &vao,//
                        GLuint &vbo,//
                        GLuint &ebo) {
  spdlog::info("Creating buffers");

  // VAO
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  glGenBuffers(1, &vbo);
  glGenBuffers(1, &ebo);

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, s.vertex_storage_sz_bytes, nullptr, GL_DYNAMIC_DRAW);

  long offset = 0;
  glEnableVertexAttribArray(POS_ATTR);
  glVertexAttribPointer(POS_ATTR, 3, GL_FLOAT, GL_FALSE, s.bytes_per_vertex, (GLvoid *) offset);
  offset += s.position_data_size;

  if (s.normal_data_size > 0) {
    glEnableVertexAttribArray(NORM_ATTR);
    glVertexAttribPointer(NORM_ATTR, 3, GL_FLOAT, GL_FALSE, s.bytes_per_vertex, (GLvoid *) offset);
    offset += s.normal_data_size;
  }

  if (s.colour_data_size > 0) {
    glEnableVertexAttribArray(COLR_ATTR);
    glVertexAttribPointer(COLR_ATTR, 3, GL_FLOAT, GL_FALSE, s.bytes_per_vertex, (GLvoid *) offset);
    offset += s.colour_data_size;
  }

  if (s.texture_coord_size > 0) {
    glEnableVertexAttribArray(TEXT_ATTR);
    glVertexAttribPointer(TEXT_ATTR, 2, GL_FLOAT, GL_FALSE, s.bytes_per_vertex, (GLvoid *) offset);
  }

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, s.index_storage_sz_bytes, nullptr, GL_DYNAMIC_DRAW);

  CHECK_GL_ERROR("create geometry")
}

/**
 * Stash the actual vertex data in the buffers
 */
void load_geometry(GLuint vao, GLuint vbo, GLuint ebo,//
                   const std::vector<float> &vertex_data,//
                   const std::vector<uint32_t> &index_data
) {
  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr) (vertex_data.size() * sizeof(float)), vertex_data.data(), GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr) (index_data.size() * sizeof(uint32_t)), index_data.data(),
               GL_STATIC_DRAW);

  CHECK_GL_ERROR("load geometry")
}

/* ******************************************************************************************
 *
 *  Create and bind Shader uniforms
 *
 * ******************************************************************************************/
std::shared_ptr<Shader> init_shader() {
  auto shader = Shader::from_files("/Users/dave/Projects/FluidSim/renderer/pool/shaders/lighting.vert",
                                   "/Users/dave/Projects/FluidSim/renderer/pool/shaders/lighting.frag");
  shader->use();
  shader->set_uniform("light_dir", glm::vec3(2, -10, 15));
  shader->set_uniform("light_intensity", 1.0f);


  CHECK_GL_ERROR("Create shader")

  return shader;
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

  // Set up callbacks
  //
  glfwSetMouseButtonCallback(window, mouse_button_callback);
  glfwSetCursorPosCallback(window, mouse_move_callback);
  glfwSetKeyCallback(window, key_callback);


  // Textures
  auto texture = std::make_shared<Texture>("/Users/dave/Projects/FluidSim/renderer/pool/assets/combo_1024x1024.jpg");

  // Set up Height field
  HeightField hf(30, 40);
  hf.Init(HeightField::PULSE);
  GeometryHelper gh{0.25f, 0.25f, true, false, true};

  glfwSetWindowUserPointer(window, &hf);

  // Create initial geometry
  GLuint vao, vbo, ebo;
  auto sn = gh.ComputeStorageNeeds(hf);
  create_geometry_buffers(sn, vao, vbo, ebo);

  // Generate and load the geometry
  std::vector<float> vertex_data;
  std::vector<uint32_t> index_data;
  gh.GenerateGeometry(hf, vertex_data, index_data);
  load_geometry(vao, vbo, ebo, vertex_data, index_data);

  // Create a shader
  auto shader = init_shader();

  auto start = std::chrono::high_resolution_clock::now();
  while (!glfwWindowShouldClose(window)) {
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    auto ratio = (float) width / (float) height;
    glViewport(0, 0, width, height);
    glm::mat4 project = glm::perspective(glm::radians(35.0f), ratio, 0.1f, 35.0f);

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.3, 0.3, 0.3, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Bind VAO
    glBindVertexArray(vao);

    // Update view
    glm::mat4 view{1};
    view = glm::translate(view, glm::vec3(0, -2.5, -20));
    view = glm::rotate_slow(view, (float) M_PI / 9.0f, {1, 0, 0});

    glm::mat4 model = glm::mat4(1.0);
    //  model = glm::translate(model,glm::vec3(0,0,-10));
    model = model * g_model_rot;

    texture->BindToTextureUnit(0);
    CHECK_GL_ERROR("Texture")

    shader->use();
    shader->set_uniform("project", project);
    shader->set_uniform("view", view);
    shader->set_uniform("model", model);
    shader->set_uniform("combi_texture", 0);

    shader->set_uniform("kd", 0.8f);
    shader->set_uniform("ks", 0.9f);
    shader->set_uniform("ka", 0.2f);
    shader->set_uniform("alpha", 30.0f);
    glDrawElements(GL_TRIANGLES, sn.water_elements, GL_UNSIGNED_INT, (void *) nullptr);
    CHECK_GL_ERROR("Render Water")

    shader->set_uniform("kd", 0.8f);
    shader->set_uniform("ks", 0.1f);
    shader->set_uniform("ka", 0.3f);
    shader->set_uniform("alpha", 1.f);
    glDrawElements(GL_TRIANGLES, sn.total_elements - sn.water_elements, GL_UNSIGNED_INT,
                   (void *) (sn.water_elements * sizeof(GLuint)));
    CHECK_GL_ERROR("Render Pool")

    //
    // Update geometry
    //
    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float, std::milli> elapsed = finish - start;
    start = finish;
    hf.Simulate(elapsed.count()/1000.0f);
    gh.GenerateGeometry(hf, vertex_data, index_data);
    load_geometry(vao, vbo, ebo, vertex_data, index_data);

    //
    // End of frame
    //
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}

