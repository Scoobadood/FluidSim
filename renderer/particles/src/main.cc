
#include <spdlog/spdlog.h>
#include <spdlog/cfg/env.h>
#include <random>

#include "window.h"
#include "arcball.h"
#include "gl_common.h"
#include "shader.h"
#include "gl_error.h"
#include "particle_system.h"
#include "ode_solver.h"

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
          "/Users/dave/Projects/FluidSim/renderer/particles/shaders/basic.vert",
          "/Users/dave/Projects/FluidSim/renderer/particles/shaders/basic.frag");
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

  Window window(800, 600, "Party");
  g_arcball = new ArcBall(800, 600);
  setup_arcball(window);


  glEnable(GL_PROGRAM_POINT_SIZE);

  auto shader = init_shader();

  // Setup GL stuff
  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  GLuint vbo;
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 24, (GLvoid *) nullptr);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 24, (GLvoid *) 12);
  CHECK_GL_ERROR("Alloc buffers")

  ParticleFactory pf;
  ParticleSystem ps{500, pf};

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
    view = glm::translate(view, glm::vec3(0, -5, -50));
    view = view * g_arcball->Rotation();


    shader->use();
    shader->set_uniform("project", project);
    shader->set_uniform("view", view);
    shader->set_uniform("model", glm::mat4(1));

    // Draw particles
    std::vector<float> particle_data;
    for (const auto &p: ps.Particles()) {
      particle_data.insert(particle_data.end(), {p->Position().x, p->Position().y, p->Position().z});
      particle_data.insert(particle_data.end(), {p->Colour().r, p->Colour().g, p->Colour().b});
    }
    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr) (particle_data.size() * sizeof(float)), particle_data.data(),
                 GL_DYNAMIC_DRAW);

    glDrawArrays(GL_POINTS, 0, ps.NumParticles());
    CHECK_GL_ERROR("Draw")

    // Check elapsed time
    auto now = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> delta_time = now - last_time_s;
    last_time_s = now;
    float dts = delta_time.count();

    // Update the physics
    euler_solve(ps, dts);

    // Constrain em to stay on screen
    ps.Constrain();

    // End of frame
    window.SwapBuffers();
  }
  return 0;
}

