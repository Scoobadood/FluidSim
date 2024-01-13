
#include "spdlog/spdlog.h"
#include "spdlog/cfg/env.h"
#include "jos_stam_2d.h"

#include <memory>
#include <GLHelpers/GLHelpers.h>
#include <common/common.h>
#include <glfw_utils/window.h>

/*
 * Clear render viewport for next pass
 */
void clear_window(Window &window) {
  int width, height;
  window.GetFrameBufferSize(width, height);
  glViewport(0, 0, width, height);

  glEnable(GL_DEPTH_TEST);
  glClearColor(0.7, 0.7, 0.9, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

int main(int argc, char *argv[]) {
  spdlog::cfg::load_env_levels();

  Window window(800, 600, "Stam3D");
//  g_arcball = new ArcBall(800, 600);
//  setup_arcball(window);

// We use a basic texture shader to render the image as a quad
  auto shader = Shader::from_files(//
      "/Users/dave/Projects/FluidSim/renderer/apps/jos_stam_3d/shaders/basic.vert",
      "/Users/dave/Projects/FluidSim/renderer/apps/jos_stam_3d/shaders/basic.frag");
  CHECK_GL_ERROR("Create shader")

  /* ************************************************************************************************
   * **
   * **   Make fluid sim
   * **
   * ************************************************************************************************/
  const uint32_t GRID_SIZE = 35;
  auto sim = std::make_shared<JosStam2D>(GRID_SIZE, 1.0f * GRID_SIZE);

  // Set up a source and forces
  std::vector<float> source((GRID_SIZE + 2) * (GRID_SIZE + 2), 0);
  std::vector<float> forcex((GRID_SIZE + 2) * (GRID_SIZE + 2), 0);
  std::vector<float> forcey((GRID_SIZE + 2) * (GRID_SIZE + 2), 0);
  source.at((GRID_SIZE + 2) * ((GRID_SIZE + 2) / 2) + ((GRID_SIZE + 2) / 2)) = 100.0f;
  forcex.at((GRID_SIZE + 2) * ((GRID_SIZE + 2) / 2) + ((GRID_SIZE + 2) / 2) - 2) = 3.0f;
  forcey.at((GRID_SIZE + 2) * ((GRID_SIZE + 2) / 2) + ((GRID_SIZE + 2) / 2) - 2) = -1.0f;

  /* ************************************************************************************************
   * **
   * **   Make a quad to render
   * **
   * ************************************************************************************************/
  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  GLuint vbo;
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  float verts[] = {-1, 1, 0, 0, 1,
                   1, 1, 0, 1, 1,
                   1, -1, 0, 1, 0,
                   -1, 1, 0, 0, 1,
                   1, -1, 0, 1, 0,
                   -1, -1, 0, 0, 0
  };
  glBufferData(GL_ARRAY_BUFFER, 6 * 5 * sizeof(float), verts, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 20, (GLvoid *) nullptr);
  glEnableVertexAttribArray(1);// Texture coords
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 20, (GLvoid *) 12);
  CHECK_GL_ERROR("Alloc buffers")

  /* ************************************************************************************************
   * **
   * **   Make a texture and some image data to hold the render
   * **
   * ************************************************************************************************/
  auto texture = std::make_shared<Texture>(GRID_SIZE, GRID_SIZE);
  std::vector<uint8_t> image_data(GRID_SIZE * GRID_SIZE * 3, 0);

  /* ************************************************************************************************
   * **
   * **   Main render loop
   * **
   * ************************************************************************************************/
  glm::mat4 r(1);
  //  r = glm::rotate_slow(r,(float)M_PI/2.0f, glm::vec3{0,0,1});
  //  g_arcball->SetRotation(r);

  auto timer = std::make_shared<FrameTimer>();
  while (!window.ShouldClose()) {
    clear_window(window);

    int32_t width, height;
    window.GetFrameBufferSize(width, height);
    auto ratio = (float) width / (float) height;
    glm::mat4 project = glm::perspective(glm::radians(35.0f), ratio, 0.1f, 1000.0f);

    /* ************************************************************************************************
     * **
     * **   Render the fluid sim into the image data and load to texture
     * **
     * ************************************************************************************************/
    auto data = sim->dens_;
    image_data.clear();
    auto data_plane_size = (GRID_SIZE + 2) * (GRID_SIZE + 2);
    auto min_d = std::numeric_limits<float>::max();
    auto max_d = std::numeric_limits<float>::lowest();
    for (auto idx = 0; idx < data_plane_size; ++idx) {
      auto d = data[idx];
      if (d < min_d) min_d = d;
      if (d > max_d) max_d = d;
    }
    auto scale = 255.0f / (max_d - min_d);
    for (auto y = 1; y <= GRID_SIZE; ++y) {
      for (auto x = 1; x <= GRID_SIZE; ++x) {
        auto idx = y * (GRID_SIZE + 2) + x;
        auto d = data[idx];
//        auto v = (uint8_t) std::fminf(255.0f, (d - min_d) * scale);
        auto v = (uint8_t) std::fminf(255.0f, std::floorf(d * 255.0f));
        image_data.push_back(v);
        image_data.push_back(v);
        image_data.push_back(v);
      }
    }
    texture->SetImageData(image_data.data());

    /* ************************************************************************************************
     * **
     * **   Draw the texture
     * **
     * ************************************************************************************************/
    shader->use();
    shader->set_uniform("project", project);
    glm::mat4 view{1};
    view = glm::translate(view, glm::vec3(0, 0, -5));
//    view = view * g_arcball->Rotation();
    shader->set_uniform("view", view);
    shader->set_uniform("model", glm::mat4(1));
    texture->BindToTextureUnit(0);
    shader->set_uniform("tx", 0);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    CHECK_GL_ERROR("Draw")


    /* ************************************************************************************************
     * **
     * **   Udpate the physics sim.
     * **
     * ************************************************************************************************/
    auto elapsed = timer->ElapsedTime();
    sim->simulate(elapsed,
                  forcex.data(), forcey.data(),
                  source.data(), 0.0, 0.5f);

    // End of frame
    window.SwapBuffers();
  }
  return 0;
}
