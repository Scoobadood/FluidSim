
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
  window.SetMouseMoveHandler([](float mouse_x, float mouse_y) {
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

  glm::mat4 r(1);
//  r = glm::rotate_slow(r,(float)M_PI/2.0f, glm::vec3{0,0,1});
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
    glm::mat4 project = glm::perspective(glm::radians(35.0f), ratio, 0.1f, 1000.0f);
    glm::mat4 view{1};
    view = glm::translate(view, glm::vec3(0, 0, -6));
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

