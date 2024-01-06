#include "spdlog/cfg/env.h"
#include "spdlog/spdlog.h"

#include <random>
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
  // Collision detection. right now we use dumb O(n2) pairwise testing to get code working
  for (auto idx = 0; idx < properties.size() - 1; ++idx) {
    const auto &p1 = properties[idx];
    const auto &min_1 = p1->AABBMinVertex();
    const auto &max_1 = p1->AABBMaxVertex();
    for (auto idx2 = idx + 1; idx2 < properties.size(); ++idx2) {
      const auto &p2 = properties[idx2];
      const auto &min_2 = p2->AABBMinVertex();
      const auto &max_2 = p2->AABBMaxVertex();

      if (max_1.x < min_2.x) continue;
      if (max_1.y < min_2.y) continue;
      if (max_1.z < min_2.z) continue;
      if (max_2.x < min_1.x) continue;
      if (max_2.y < min_1.y) continue;
      if (max_2.z < min_1.z) continue;

      // BB collision. We'll be naive here and take the intersection points midpoint

    }
  }
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

  glm::vec3 spawn_point{0, 10, 0};
  std::vector<std::shared_ptr<PhysProperties>> prop_list;
  auto cube_properties = std::make_shared<PhysProperties>();
  cube_properties->SetPosition(spawn_point);
  const auto &com = cube_mesh->CentreOfMass();
  cube_properties->SetCentreOfMass(com[0], com[1], com[2]);
  const auto &bb = cube_mesh->Bounds();
  cube_properties->SetBoundingBox(bb.min_vertex[0], bb.min_vertex[1], bb.min_vertex[2],
                                  bb.max_vertex[0], bb.max_vertex[1], bb.max_vertex[2]
  );
  prop_list.push_back(cube_properties);

  auto shader = init_shader();

  auto last_time_s = std::chrono::high_resolution_clock::now();
  glm::mat4 r(1);
//  r = glm::rotate_slow(r,(float)M_PI/2.0f, glm::vec3{0,0,1});
  g_arcball->SetRotation(r);

  // Rando Kick
  const unsigned int seed = 123;
  std::mt19937_64 rng(seed);
  std::uniform_int_distribution<int> rand_time(1, 100);
  std::uniform_real_distribution<float> rand_vec(-1500,1500);
  // Remove Me
  bool kicked = false;
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
    auto model = cube_properties->ModelToWorldMatrix();

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

    if (!kicked && rand_time(rng) == 1) {
      kicked = true;
      // gen random force unit vector
      glm::vec3 whump{rand_vec(rng), 1500, rand_vec(rng)};
      cube_properties->ApplyForce({0,0,0}, whump);
    }

    auto elapsed_s = elapsed.count();
    update_physics(elapsed_s, mesh_list, prop_list);

    auto p = cube_properties->Position();

    if ( p.y < -10) {
      kicked = false;
      cube_properties->SetPosition(spawn_point);
      cube_properties->SetVelocity({0, 0, 0});
    }

    // End of frame
    window.SwapBuffers();
  }
  return 0;
}

