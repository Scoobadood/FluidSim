
#include "spdlog/spdlog.h"
#include "spdlog/cfg/env.h"
#include "grid_fluid_simulator_3d.h"

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
   * **   Make fluid sim
   * **
   * ************************************************************************************************/
  const int GRID_WIDTH = 64;
  const int GRID_HEIGHT = 64;
  const int GRID_DEPTH = 3;
  auto sim = std::make_shared<GridFluidSimulator3D>(GRID_WIDTH,
                                                    GRID_HEIGHT,
                                                    GRID_DEPTH,
                                                    1.0f * GRID_WIDTH,
                                                    1.0f * GRID_HEIGHT,
                                                    1.0f * GRID_DEPTH,
                                                    0.0f,
                                                    0.0f,
                                                    0.00001f);

  // Set up a source
  auto mid_idx = (GRID_WIDTH * GRID_HEIGHT * (GRID_DEPTH / 2)) + (GRID_WIDTH * (GRID_HEIGHT / 2)) + (GRID_WIDTH / 2);
  std::vector<float> source(GRID_WIDTH * GRID_HEIGHT * GRID_DEPTH, 0);
  std::vector<vec3f> force(GRID_WIDTH * GRID_HEIGHT * GRID_DEPTH, vec3f{0.0f, 0.0f, 0.0f});
  source.at(mid_idx) = 10.0f;
  force.at(mid_idx - (GRID_WIDTH / 4)) = {0.0f, 0.0001f, 0.0f};

  /* ************************************************************************************************
   * **
   * **   Make a texture to hold the render
   * **
   * ************************************************************************************************/
  auto texture = std::make_shared<Texture>(GRID_WIDTH, GRID_HEIGHT);

  // Allocate image data to which to render.
  std::vector<uint8_t> image_data(GRID_WIDTH * GRID_HEIGHT * 3, 0);

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
    auto data = sim->Density();
    image_data.clear();
    // Planar offset for middle of sim
    auto data_plane_size = GRID_WIDTH * GRID_HEIGHT;
    auto mid_plane_offset = data_plane_size * (GRID_DEPTH / 2);

    auto min_d = std::numeric_limits<float>::max();
    auto max_d = std::numeric_limits<float>::lowest();
    for (auto idx = 0; idx < data_plane_size; ++idx) {
      auto d = data.at(idx + mid_plane_offset);
      if (d < min_d) min_d = d;
      if (d > max_d) max_d = d;
    }
    auto scale = 255.0f / (max_d - min_d);
    for (auto idx = 0; idx < data_plane_size; ++idx) {
      auto d = data.at(idx + mid_plane_offset);
      auto v = (uint8_t) std::fminf(255.0f, (d - min_d) * scale);
      image_data.push_back(v);
      image_data.push_back(v);
      image_data.push_back(v);
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
    sim->Simulate(source, force, elapsed);

    // End of frame
    window.SwapBuffers();
  }
  return 0;
}
