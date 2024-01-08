
#include "spdlog/spdlog.h"
#include "spdlog/cfg/env.h"
#include "renderer.h"
#include "frame_timer.h"
#include "input_mgr.h"
#include <random>

#include <GLHelpers/GLHelpers.h>
#include <particles/particles.h>
#include <common/common.h>
#include <glfw_utils/window.h>

/* ******************************************************************************************
 *
 *  Main
 *
 * ******************************************************************************************/
int main(__attribute__((unused)) int argc, __attribute__((unused)) char *argv[]) {
  spdlog::cfg::load_env_levels();

  Window window(800, 600, "Party");
  auto input_manager = std::make_shared<InputManager>(800,600);

  glEnable(GL_PROGRAM_POINT_SIZE);

  /*
   * Particle system init
   *
   */
  auto  pf = std::make_shared<ParticleFactory>();
  auto ps = std::make_shared<ParticleSystem>(5, pf);
  auto &particles = ps->Particles();
  particles[0]->SetPosition({-2, -2, 0});
  particles[1]->SetPosition({2, -2, 0});
  particles[2]->SetPosition({2, 2, 0});
  particles[3]->SetPosition({-2, 2, 0});
  particles[4]->SetPosition({0, 0, 0});

  particles[4]->SetMass(-1.0f);

//  ps.AddForceHandler(std::make_shared<GlobalForceHandler>(glm::vec3{0, -9.8f, 0}));

// Tie them into a chain
  const float SF = 10.0f;
  const float DP = 5.f;
  const float RL = 3.f;
  const float RLD = std::sqrtf(RL * RL * 0.25f);
  const float SFD = 5.0f;

  ps->AddForceHandler(std::make_shared<SpringForceHandler>(particles[0], particles[1], RL, SF, DP));
  ps->AddForceHandler(std::make_shared<SpringForceHandler>(particles[1], particles[2], RL, SF, DP));
  ps->AddForceHandler(std::make_shared<SpringForceHandler>(particles[2], particles[3], RL, SF, DP));
  ps->AddForceHandler(std::make_shared<SpringForceHandler>(particles[3], particles[0], RL, SF, DP));

  ps->AddForceHandler(std::make_shared<SpringForceHandler>(particles[0], particles[4], RLD, SFD, DP));
  ps->AddForceHandler(std::make_shared<SpringForceHandler>(particles[1], particles[4], RLD, SFD, DP));
  ps->AddForceHandler(std::make_shared<SpringForceHandler>(particles[2], particles[4], RLD, SFD, DP));
  ps->AddForceHandler(std::make_shared<SpringForceHandler>(particles[3], particles[4], RLD, SFD, DP));

  auto vdh = std::make_shared<ViscousDragHandler>(1.f);
  ps->AddForceHandler(vdh);

  CHECK_GL_ERROR("init world")

  glm::mat4 project{1};
  glm::vec3 cam_pos{0, 0, 6};
  glm::mat4 view{1};

  auto frame_timer = std::make_shared<FrameTimer>();

  int32_t width, height;
  window.GetFrameBufferSize(width, height);
  auto renderer = std::make_shared<Renderer>(width, height);
  renderer->InitShaders();

  while (!window.ShouldClose()) {

    renderer->Render(ps, g_arcball->Rotation());

    auto dts = frame_timer->ElapsedTime();

    // Update the physics
    euler_solve(ps, dts);

    // End of frame
    window.SwapBuffers();
  }
  return 0;
}

