
#include "spdlog/spdlog.h"
#include "spdlog/cfg/env.h"
#include "grid_fluid_simulator_3d.h"

#include <GLHelpers/GLHelpers.h>
#include <common/common.h>
#include <glfw_utils/window.h>

void clear_window( Window &window) {
  int width, height;
  window.GetFrameBufferSize(width, height);
  glViewport(0, 0, width, height);

  glEnable(GL_DEPTH_TEST);
  glClearColor(0.7, 0.7, 0.9, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

int main(int argc, char *argv[]) {
  spdlog::cfg::load_env_levels();

  Window window(800, 600, "Party");
//  g_arcball = new ArcBall(800, 600);
//  setup_arcball(window);

  glEnable(GL_PROGRAM_POINT_SIZE);

  auto shader = Shader::from_files(//
      "/Users/dave/Projects/FluidSim/renderer/apps/party/shaders/basic.vert",
      "/Users/dave/Projects/FluidSim/renderer/apps/party/shaders/basic.frag");
  CHECK_GL_ERROR("Create shader")

  /* ************************************************************************************************
   * **
   * **   Make a quad to render
   * **
   * ************************************************************************************************/
  // Setup GL stuff
  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  GLuint vbo;
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  float verts[] = {-1, 1, 1,
                   -1, 1, -1,
                   -1, -1, -1,
                   -1, -1, 1,
                   1, 1, 1,
                   1, 1, -1,
                   1, -1, -1,
                   1, -1, 1
  };
  glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(float), verts, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 24, (GLvoid *) nullptr);
  CHECK_GL_ERROR("Alloc buffers")


  /* ************************************************************************************************
   * **
   * **   Make fluid sim
   * **
   * ************************************************************************************************/
  const int GRID_SIZE = 10;
  auto sim = std::make_shared<GridFluidSimulator>(GRID_SIZE, GRID_SIZE, 0.1, 0.1);

  glm::mat4 r(1);
//  r = glm::rotate_slow(r,(float)M_PI/2.0f, glm::vec3{0,0,1});
//  g_arcball->SetRotation(r);

  auto timer = std::make_shared<FrameTimer>();
  auto last_time_s = std::chrono::high_resolution_clock::now();
  while (!window.ShouldClose()) {
    clear_window(window);

    int32_t width, height;
    window.GetFrameBufferSize(width, height);
    auto ratio = (float) width / (float) height;
    glm::mat4 project = glm::perspective(glm::radians(35.0f), ratio, 0.1f, 1000.0f);

    shader->use();
    shader->set_uniform("project", project);
    glm::mat4 view{1};
    view = glm::translate(view, glm::vec3(0, 0, -10));
//    view = view * g_arcball->Rotation();

    shader->set_uniform("view", view);
    shader->set_uniform("model", glm::mat4(1));

    glDrawArrays(GL_POINTS, 0, 24);
    CHECK_GL_ERROR("Draw")

    // Check elapsed time
    auto elapsed = timer->ElapsedTime();
    sim->Simulate()

    // End of frame
    window.SwapBuffers();
  }
  return 0;
}
