#include "spdlog/cfg/env.h"
#include "spdlog/spdlog.h"

#include "gl_common.h"
#include "shader.h"
#include "arcball.h"
#include "gl_error.h"
#include "window.h"
#include "mesh.h"
#include "mesh_helper.h"
#include "texture.h"

ArcBall *g_arcball;

/* ******************************************************************************************
 *
 *  Create and bind Shader uniforms
 *
 * ******************************************************************************************/
std::shared_ptr<Shader> init_shader() {
  auto shader = Shader::from_files("/Users/dave/Projects/FluidSim/renderer/physx/shaders/basic.vert",
                                   "/Users/dave/Projects/FluidSim/renderer/physx/shaders/basic.frag");
  CHECK_GL_ERROR("Create shader")
  return shader;
}

void setup_input_handlers(Window &window) {
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

void DoPhysics(std::vector<Mesh>& meshes) {

}

/* ******************************************************************************************
 *
 *  Main
 *
 * ******************************************************************************************/
int main(__attribute__((unused)) int argc, __attribute__((unused)) char *argv[]) {
  spdlog::cfg::load_env_levels();

  Window window(800, 600, "PhysX");
  g_arcball = new ArcBall(800, 400);
  setup_input_handlers(window);

  Mesh cube_mesh = MeshHelper::Cuboid(1,1,1,0,0,0,1,0,0);

  auto shader = init_shader();

  g_arcball->SetRotation(glm::rotate_slow(glm::mat4(1), (float) M_PI / 9.0f, {1, 0, 0}));

  auto last_time_s = std::chrono::high_resolution_clock::now();
  while (!window.ShouldClose()) {
    int width, height;
    window.GetFrameBufferSize(width, height);
    glViewport(0, 0, width, height);

    auto ratio = (float) width / (float) height;
    glm::mat4 project = glm::perspective(glm::radians(35.0f), ratio, 0.1f, 100.0f);

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.2, 0.2, 0.2, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Update view
    glm::mat4 view{1};
    view = glm::translate(view, glm::vec3(0, -.1, -5));
    view = view * g_arcball->Rotation();

    glm::mat4 model = glm::mat4(1.0);

    shader->use();
    shader->set_uniform("project", project);
    shader->set_uniform("view", view);
    shader->set_uniform("model", model);
    shader->set_uniform("light_dir", {0,-1,0});

    cube_mesh.Bind();
    glDrawElements(GL_TRIANGLES, cube_mesh.NumElements(), GL_UNSIGNED_INT, (void *) nullptr);

    // Check elapsed time
    auto now = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> elapsed = now - last_time_s;
    last_time_s = now;

    // End of frame
    window.SwapBuffers();
  }
  return 0;
}

