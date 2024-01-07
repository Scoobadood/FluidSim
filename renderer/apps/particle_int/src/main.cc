
#include "spdlog/spdlog.h"
#include "spdlog/cfg/env.h"
#include <random>

#include <GLHelpers/GLHelpers.h>
#include <particles/particles.h>
#include <common/common.h>
#include <glfw_utils/window.h>

ArcBall *g_arcball;

void setup_arcball(Window &window) {
  window.SetRightMousePressHandler([](float mouse_x, float mouse_y) {
    g_arcball->DragStarted(mouse_x, mouse_y);
  });
  window.SetRightMouseReleaseHandler([](float mouse_x, float mouse_y) {
    g_arcball->DragEnded();
  });
  window.SetRightMouseDragHandler([](float mouse_x, float mouse_y) {
    g_arcball->Drag(mouse_x, mouse_y);
  });
}

std::shared_ptr<Shader> init_shader() {
  auto shader = Shader::from_files(//
      "/Users/dave/Projects/FluidSim/renderer/apps/party/shaders/basic.vert",
      "/Users/dave/Projects/FluidSim/renderer/apps/party/shaders/basic.frag");
  CHECK_GL_ERROR("Create shader")
  return shader;
}

/*
 * Create five particles with springs
 */
void init_gl_buffers(GLuint &vao, GLuint &vbo_masses, GLuint &ebo_springs) {
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  GLuint buff[2];
  glGenBuffers(2, buff);
  vbo_masses = buff[0];
  glBindBuffer(GL_ARRAY_BUFFER, vbo_masses);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 24, (GLvoid *) nullptr);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 24, (GLvoid *) 12);

  ebo_springs = buff[1];
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_springs);

  CHECK_GL_ERROR("Alloc buffers")
}

/*
 * Convert a mouse click into a ray in world space coordinates
 * Convert Mouse Coordinates to Normalized Device Coordinates (NDC):
 *  Transform the mouse screen coordinates to NDC space (-1 to 1 range).
 * Create Ray in View Space:
 *  Transform the NDC coordinates to view space. For a perspective camera, this involves reversing the projection matrix.
 *  For an orthographic camera, it's a simpler transformation.
 *  Transform Ray to World Space:
 *    Transform the ray from view space to world space using the inverse of the camera's view matrix.
 */
glm::vec3 mouse_to_ray(uint32_t width,
                       uint32_t height,
                       glm::mat4 &proj,
                       glm::mat4 &view,
                       uint32_t xpos,
                       uint32_t ypos,
                       glm::vec3 &near,
                       glm::vec3 &far) {
  auto ndc_x = (2.0f * ((float) xpos / (float) width)) - 1.0f;
  auto ndc_y = 1.0f - (2.0f * ((float) ypos / (float) height));
  glm::vec4 ray_start_ndc{ndc_x, ndc_y, -1.0f, 1.0f};
  glm::vec4 ray_end_ndc{ndc_x, ndc_y, 1.0f, 1.0f};

  auto inv_proj = glm::inverse(proj);
  auto ray_start_view = inv_proj * ray_start_ndc;
  auto ray_end_view = inv_proj * ray_end_ndc;
  ray_start_view /= ray_start_view.w;
  ray_end_view /= ray_end_view.w;

  auto inv_view = glm::inverse(view);
  auto ray_start_world = inv_view * ray_start_view;
  auto ray_end_world = inv_view * ray_end_view;
  ray_start_world /= ray_start_world.w;
  ray_end_world /= ray_end_world.w;
  glm::vec3 ray_dir = (ray_end_world - ray_start_world);
  near = ray_start_world;
  far = ray_end_world;
  return glm::normalize(ray_dir);
}

float point_to_line_dist(const glm::vec3 &P, const glm::vec3 &A, const glm::vec3 &B) {
  using namespace glm;
  // L1 to point
  vec3 AB = B - A;
  vec3 AP = P - A;

  vec3 cp = glm::cross(AP, AB);
  // Magnitude of the pll gram
  float area = glm::length(cp);
  // This is also perp dist * base (|AB|)
  float base = glm::length(AB);
  float perp_dist = area / base;
  return perp_dist;
}

glm::vec3 closest_point_on_ray(const glm::vec3 &P, const glm::vec3 &A, const glm::vec3 &B) {
  using namespace glm;
  // L1 to point
  vec3 AB = B - A;
  vec3 AP = P - A;

  auto dp1 = glm::dot(AP, AB);
  auto dp2 = glm::dot(AB, AB);

  return A + ((dp1 / dp2) * AB);
}

int32_t index_of_closest_point(const glm::vec3 &near, const glm::vec3 &far, const std::vector<float> &points) {
  auto num_points = points.size() / 3;
  auto index = -1;
  float min_dist = 10.0f;
  for (auto i = 0; i < num_points; i++) {
    auto dist =
        point_to_line_dist({points[i * 3], points[i * 3 + 1], points[i * 3 + 2]}, near, far);
    if (dist < min_dist) {
      min_dist = dist;
      index = i;
    }
  }
  return index;
}

/* ******************************************************************************************
 *
 *  Global draggy things that are to be fixed later
 *
 * ******************************************************************************************/
bool g_is_dragging = false;
int32_t g_drag_idx = -1;
float g_drag_sprint_length;
std::shared_ptr<SpringForceHandler> g_drag_spring;
glm::vec3 g_drag_point;
std::shared_ptr<Particle> g_drag_particle;

/* ******************************************************************************************
 *
 *  Main
 *
 * ******************************************************************************************/
int main(__attribute__((unused)) int argc, __attribute__((unused)) char *argv[]) {
  spdlog::cfg::load_env_levels();

  Window window(800, 600, "Party");
  g_arcball = new ArcBall(800, 600);
  setup_arcball(window);

  glEnable(GL_PROGRAM_POINT_SIZE);

  auto shader = init_shader();

  // Setup GL stuff
  GLuint vao, vbo_masses, ebo_springs;
  init_gl_buffers(vao, vbo_masses, ebo_springs);

  /*
   * Particle system init
   *
   */
  std::shared_ptr<ParticleFactory> pf = std::make_shared<ParticleFactory>();
  ParticleSystem ps{5, pf};
  auto &particles = ps.Particles();
  particles[0]->SetPosition({-2, -2, 0});
  particles[1]->SetPosition({2, -2, 0});
  particles[2]->SetPosition({2, 2, 0});
  particles[3]->SetPosition({-2, 2, 0});
  particles[4]->SetPosition({0, 0, 0});

//  ps.AddForceHandler(std::make_shared<GlobalForceHandler>(glm::vec3{0, -9.8f, 0}));

// Tie them into a chain
  const float SF = 10.0f;
  const float DP = 5.f;
  const float RL = 3.f;
  const float RLD = std::sqrtf(RL * RL * 0.25f);
  const float SFD = 5.0f;

  ps.AddForceHandler(std::make_shared<SpringForceHandler>(particles[0], particles[1], RL, SF, DP));
  ps.AddForceHandler(std::make_shared<SpringForceHandler>(particles[1], particles[2], RL, SF, DP));
  ps.AddForceHandler(std::make_shared<SpringForceHandler>(particles[2], particles[3], RL, SF, DP));
  ps.AddForceHandler(std::make_shared<SpringForceHandler>(particles[3], particles[0], RL, SF, DP));

  ps.AddForceHandler(std::make_shared<SpringForceHandler>(particles[0], particles[4], RLD, SFD, DP));
  ps.AddForceHandler(std::make_shared<SpringForceHandler>(particles[1], particles[4], RLD, SFD, DP));
  ps.AddForceHandler(std::make_shared<SpringForceHandler>(particles[2], particles[4], RLD, SFD, DP));
  ps.AddForceHandler(std::make_shared<SpringForceHandler>(particles[3], particles[4], RLD, SFD, DP));

//  auto vdh =std::make_shared<ViscousDragHandler>(1.f);
//  ps.AddForceHandler(vdh);

  CHECK_GL_ERROR("init world")

  glm::mat4 project{1};
  glm::vec3 cam_pos{0, 0, 6};
  glm::mat4 view{1};

  window.SetLeftMousePressHandler([&project, &view, &ps](float mouse_x, float mouse_y) {
    glm::vec3 near, far;
    auto ray = mouse_to_ray(800,
                            600,
                            project,
                            view,
                            (uint32_t) std::roundf(mouse_x),
                            (uint32_t) std::roundf(mouse_y),
                            near,
                            far);
    g_drag_idx = index_of_closest_point(near, far, ps.Positions());
    spdlog::info("Closest point index {}", g_drag_idx);
    if (g_drag_idx >= 0) {
      g_is_dragging = true;
      g_drag_point = closest_point_on_ray(ps.Particles()[g_drag_idx]->Position(), near, far);
      g_drag_particle = std::make_shared<Particle>(g_drag_point, glm::vec3{1, 1, 1}, -1.0f);
      g_drag_spring = std::make_shared<SpringForceHandler>(ps.Particles()[g_drag_idx],
                                                           g_drag_particle,
                                                           g_drag_sprint_length,
                                                           10,
                                                           1);
      ps.AddForceHandler(g_drag_spring);
    }
  });

  window.SetLeftMouseDragHandler([&project, &view, &ps](float mouse_x, float mouse_y) {
    if (!g_is_dragging) return;
    glm::vec3 near, far;
    auto ray = mouse_to_ray(800,
                            600,
                            project,
                            view,
                            (uint32_t) std::roundf(mouse_x),
                            (uint32_t) std::roundf(mouse_y),
                            near,
                            far);
    g_drag_point = closest_point_on_ray(ps.Particles()[g_drag_idx]->Position(), near, far);
    g_drag_particle->SetPosition(g_drag_point);
  });

  window.SetLeftMouseReleaseHandler([&ps](float mouse_x, float mouse_y) {
    if (!g_is_dragging) return;
    g_drag_idx = -1;
    g_is_dragging = false;
    g_drag_point = glm::vec3{0};
    ps.RemoveForceHandler(g_drag_spring);
  });

  glm::mat4 r(1);
  g_arcball->SetRotation(r);
  auto last_time_s = std::chrono::high_resolution_clock::now();

  while (!window.ShouldClose()) {
    int width, height;
    window.GetFrameBufferSize(width, height);
    glViewport(0, 0, width, height);

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.2, 0.2, 0.2, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // View Things
    auto ratio = (float) width / (float) height;
    project = glm::perspective(glm::radians(35.0f), ratio, 0.1f, 1000.0f);
    view = glm::mat4{1};
    view = glm::translate(view, -cam_pos);
    view = view * g_arcball->Rotation();

    shader->use();
    shader->set_uniform("project", project);
    shader->set_uniform("view", view);
    shader->set_uniform("model", glm::mat4(1));

    // Draw particles
    std::vector<float> particle_data;
    for (const auto &p : ps.Particles()) {
      particle_data.insert(particle_data.end(), {p->Position().x, p->Position().y, p->Position().z});
      particle_data.insert(particle_data.end(), {p->Colour().r, p->Colour().g, p->Colour().b});
    }
    glBufferData(GL_ARRAY_BUFFER,
                 (GLsizeiptr) (particle_data.size() * sizeof(float)),
                 particle_data.data(),
                 GL_DYNAMIC_DRAW);
    uint32_t lines[] = {0, 1, 1, 2, 2, 3, 3, 0, 0, 4, 1, 4, 2, 4, 3, 4};
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 8 * 2 * sizeof(uint32_t),
                 lines,
                 GL_DYNAMIC_DRAW);

    glDrawArrays(GL_POINTS, 0, ps.NumParticles());
    glDrawElements(GL_LINES, 16 * 2, GL_UNSIGNED_INT, nullptr);
    CHECK_GL_ERROR("Draw")

    // Draw a surroundy box if needed
    if (g_drag_idx >= 0) {
      float box_data[4 * 6];
      auto px = particle_data.at(g_drag_idx * 6);
      auto py = particle_data.at(g_drag_idx * 6 + 1);
      auto pz = particle_data.at(g_drag_idx * 6 + 2);
      auto pr = particle_data.at(g_drag_idx * 6 + 3);
      auto pg = particle_data.at(g_drag_idx * 6 + 4);
      auto pb = particle_data.at(g_drag_idx * 6 + 5);
      // Colour all verts
      for (auto v = 0; v < 4; v++) {
        box_data[v * 6 + 0 + 2] = pz;
        box_data[v * 6 + 3 + 0] = pr;
        box_data[v * 6 + 3 + 1] = pg;
        box_data[v * 6 + 3 + 2] = pb;
      }
      box_data[0 * 6 + 0 + 0] = px - 0.1f;
      box_data[0 * 6 + 0 + 1] = py - 0.1f;

      box_data[1 * 6 + 0 + 0] = px + 0.1f;
      box_data[1 * 6 + 0 + 1] = py - 0.1f;

      box_data[2 * 6 + 0 + 0] = px + 0.1f;
      box_data[2 * 6 + 0 + 1] = py + 0.1f;

      box_data[3 * 6 + 0 + 0] = px - 0.1f;
      box_data[3 * 6 + 0 + 1] = py + 0.1f;

      glBufferData(GL_ARRAY_BUFFER,
                   (GLsizeiptr) (24 * sizeof(float)), box_data, GL_DYNAMIC_DRAW);
      glDrawElements(GL_LINES, 8, GL_UNSIGNED_INT, nullptr);
    }

    // Check elapsed time
    auto now = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> delta_time = now - last_time_s;
    last_time_s = now;
    float dts = delta_time.count();

    // Update the physics
    euler_solve(ps, dts);

    // End of frame
    window.SwapBuffers();
  }
  return 0;
}

