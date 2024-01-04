#include "spdlog/cfg/env.h"
#include "spdlog/spdlog.h"

#include "gl_common.h"
#include "gl_error.h"
#include "shader.h"
#include "texture.h"
#include "height_field_sim.h"
#include "geometry_helper.h"

#include <GLFW/glfw3.h>
#include "mesh.h"
#include "model_geometry_helper.h"
#include "window.h"
#include "arcball.h"


ArcBall *g_arcball;

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

void setup_input_handlers(Window &window, HeightField &hf) {
  window.SetRightMousePressHandler([](float mouse_x, float mouse_y) {
    g_arcball->DragStarted(mouse_x, mouse_y);
  });
  window.SetRightMouseReleaseHandler([](float mouse_x, float mouse_y) {
    g_arcball->DragEnded();
  });
  window.SetMouseMoveHandler([](float mouse_x, float mouse_y) {
    g_arcball->Drag(mouse_x, mouse_y);
  });
  window.RegisterKeyReleaseHandler(GLFW_KEY_R, [&]() { hf.Init(HeightField::PULSE); });
  window.RegisterKeyReleaseHandler(GLFW_KEY_W, [&]() { hf.Init(HeightField::WAVE); });
  window.RegisterKeyReleaseHandler(GLFW_KEY_S, [&]() { hf.Init(HeightField::CUBE); });
  window.RegisterKeyReleaseHandler(GLFW_KEY_PERIOD, [&]() { hf.IncreaseWaveSpeed(); });
  window.RegisterKeyReleaseHandler(GLFW_KEY_COMMA, [&]() { hf.DecreaseWaveSpeed(); });
  window.RegisterKeyReleaseHandler(GLFW_KEY_X, [&]() { hf.ToggleXBoundary(); });
  window.RegisterKeyReleaseHandler(GLFW_KEY_Z, [&]() { hf.ToggleZBoundary(); });
}

Mesh load_scene() {
  std::vector<float> scene_verts;
  std::vector<uint32_t> scene_indices;
  load_model_from_file("/Users/dave/Projects/FluidSim/renderer/pool/assets/pool_scene.ply", false, true, true,
                       scene_verts, scene_indices);
  Mesh scene_mesh{0, 1, -1, 3};
  scene_mesh.SetVertexData(scene_verts);
  scene_mesh.SetIndexData(scene_indices);
  return scene_mesh;
}
/* ******************************************************************************************
 *
 *  Main
 *
 * ******************************************************************************************/
int main(__attribute__((unused)) int argc, __attribute__((unused)) char *argv[]) {
  spdlog::cfg::load_env_levels();

  Window window(800, 600, "Wet");
  g_arcball = new ArcBall(800, 400);


  auto shader = init_shader();
  auto texture = std::make_shared<Texture>("/Users/dave/Projects/FluidSim/renderer/pool/assets/combi4096.png");

  // Create initial geometry
  Mesh scene_mesh = load_scene();

  // Pool dim
  float pool_x = 22.0f;
  float pool_z = 32.0f;
  float pool_depth = 4.0f;
  // Set up Height field
  HeightField hf(44, 64, pool_x, pool_z, pool_depth);
  hf.Init(HeightField::PULSE);

  GeometryHelper gh{pool_x, pool_z, 0.0f, pool_depth, true, false, true};
  std::vector<float> vertex_data;
  std::vector<uint32_t> index_data;
  gh.GenerateGeometry(hf, vertex_data, index_data, true, false);

  Mesh water_mesh{0, 1, -1, 3};
  water_mesh.SetVertexData(vertex_data);
  water_mesh.SetIndexData(index_data);

  setup_input_handlers(window, hf);

  g_arcball->SetRotation(glm::rotate_slow(glm::mat4(1), (float) M_PI / 9.0f, {1, 0, 0}));

  auto start = std::chrono::high_resolution_clock::now();
  while (!window.ShouldClose()) {
    int width, height;
    window.GetFrameBufferSize(width, height);
    auto ratio = (float) width / (float) height;
    glViewport(0, 0, width, height);
    glm::mat4 project = glm::perspective(glm::radians(35.0f), ratio, 0.1f, 100.0f);

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.3, 0.3, 0.3, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Bind VAO
    water_mesh.Bind();

    // Update view
    glm::mat4 view{1};
    view = glm::translate(view, glm::vec3(0, -6.25, -50));
    view = view * g_arcball->Rotation();

    glm::mat4 model = glm::mat4(1.0);
    //  model = glm::translate(model,glm::vec3(0,0,-10));
//    model = model * g_model_rot;

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
    water_mesh.Bind();
    glDrawElements(GL_TRIANGLES, water_mesh.NumElements(), GL_UNSIGNED_INT, (void *) nullptr);
    CHECK_GL_ERROR("Render Water")

    shader->set_uniform("kd", 0.8f);
    shader->set_uniform("ks", 0.1f);
    shader->set_uniform("ka", 0.3f);
    shader->set_uniform("alpha", 1.f);
    scene_mesh.Bind();
    glDrawElements(GL_TRIANGLES, scene_mesh.NumElements(), GL_UNSIGNED_INT, (void *) nullptr);
    CHECK_GL_ERROR("Render Pool")

    //
    // Update geometry
    //
    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float, std::milli> elapsed = finish - start;
    start = finish;
    hf.Simulate(elapsed.count() / 1000.0f);
    gh.GenerateGeometry(hf, vertex_data, index_data, true, false);

    water_mesh.SetVertexData(vertex_data);

    //
    // End of frame
    //
    window.SwapBuffers();
    glfwPollEvents();
  }
  return 0;
}

