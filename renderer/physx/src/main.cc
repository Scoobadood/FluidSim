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
#include "physics_props.h"

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

void update_physics(float delta_t, std::vector<std::shared_ptr<Mesh>> &meshes,
                    std::vector<std::shared_ptr<PhysProperties>> &properties) {
  for (auto &p: properties) {
    p->Tick(delta_t);
  }
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

  std::vector<std::shared_ptr<Mesh>> mesh_list;
  auto cube_mesh = MeshHelper::Cuboid(1, 1, 1, 0, 0, 0, 1, 0, 0);
  mesh_list.push_back(cube_mesh);

  std::vector<std::shared_ptr<PhysProperties>> prop_list;
  auto cube_properties = std::make_shared<PhysProperties>();
  cube_properties->SetPosition(0,10,0);
  prop_list.push_back(cube_properties);

  auto shader = init_shader();

  auto last_time_s = std::chrono::high_resolution_clock::now();
  glm::mat4 r;
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
    view = glm::translate(view, glm::vec3(0, 0, -100));
    view = view * g_arcball->Rotation();
    auto model= cube_properties->ModelToWorldMatrix();

    shader->use();
    shader->set_uniform("project", project);
    shader->set_uniform("view", view);
    shader->set_uniform("model", model);
    shader->set_uniform("light_dir", {0, -1, 0});

    cube_mesh->Bind();
    glDrawElements(GL_TRIANGLES, cube_mesh->NumElements(), GL_UNSIGNED_INT, (void *) nullptr);

    // Check elapsed time
    auto now = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> elapsed = now - last_time_s;
    last_time_s = now;

    cube_properties->ApplyGravity();
    auto elapsed_s =elapsed.count();
    update_physics(elapsed_s, mesh_list, prop_list);

    auto p = cube_properties->Position();
    spdlog::info(" Now at {:0.2f} {:0.2f} {:0.2f}", p.x, p.y, p.z);

    // End of frame
    window.SwapBuffers();
  }
  return 0;
}

