
#include <spdlog/spdlog.h>
#include <spdlog/cfg/env.h>
#include <random>

#include "window.h"
#include "arcball.h"
#include "gl_common.h"
#include "shader.h"
#include "gl_error.h"

ArcBall *g_arcball;

const uint32_t NUM_PARTICLES = 500;

class Particle {
public:
  explicit Particle(const glm::vec3 &position,
                    const glm::vec3 &colour,
                    float mass = 1.0f
                    ) //
          : position_{position}//
          , colour_{colour} //
  {
    inv_mass_ = 1.0f / mass;
  }

  void ApplyForce(const glm::vec3& force) {
    force_ += force;
  }

  void Resolve(float delta_t) {
    auto accel = force_ *inv_mass_;
    velocity_ += (delta_t * accel);
    position_ += (delta_t * velocity_);
    force_ = {0,0,0};
  }

  inline const glm::vec3 &Position() const { return position_; };
  inline void SetPosition(const glm::vec3& position)  { position_ = position; };

  inline const glm::vec3 &Colour() const { return colour_; };
private:
  glm::vec3 position_;
  glm::vec3 colour_;
  glm::vec3 velocity_;
  glm::vec3 force_;
  float inv_mass_;
};

std::vector<std::shared_ptr<Particle>> g_world;

glm::vec3 random_position() {
  static unsigned int seed = 123;
  static std::mt19937_64 rng{seed};
  static std::uniform_real_distribution<float> r{-100, 100};
  return {r(rng), r(rng), r(rng)};
}

glm::vec3 random_colour() {
  static unsigned int seed = 123;
  static std::mt19937_64 rng{seed};
  static std::uniform_real_distribution<float> col{0, 1.0};
  return {col(rng), col(rng), col(rng)};
}

std::shared_ptr<Shader> init_shader() {
  auto shader = Shader::from_files(//
          "/Users/dave/Projects/FluidSim/renderer/particles/shaders/basic.vert",//
          "/Users/dave/Projects/FluidSim/renderer/particles/shaders/basic.frag");
  CHECK_GL_ERROR("Create shader")
  shader->use();
  shader->set_uniform("model", glm::mat4(1));

  return shader;
}

void init_world() {
  for (auto i = 0; i < NUM_PARTICLES; i++) {
    auto p = random_position();
    auto c = random_colour();
    g_world.push_back(std::make_shared<Particle>(p, c));
  }
}

void update_world(float delta_t) {
  for (auto p : g_world ) {
    p->ApplyForce({0,-9.8f,0});
    p->Resolve(delta_t);
    if( p->Position().y < -150) {
      p->SetPosition(random_position());
    }
  }
}

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

  init_world();

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
    view = glm::translate(view, glm::vec3(0, -5, -450));
    view = view * g_arcball->Rotation();

    shader->use();
    shader->set_uniform("project", project);
    shader->set_uniform("view", view);

    // Draw particles
    std::vector<float> particle_data;
    for (const auto &p: g_world) {
      particle_data.insert(particle_data.end(), {p->Position().x, p->Position().y, p->Position().z});
      particle_data.insert(particle_data.end(), {p->Colour().r, p->Colour().g, p->Colour().b});
    }
    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr) (particle_data.size() * sizeof(float)), particle_data.data(),
                 GL_DYNAMIC_DRAW);

    glDrawArrays(GL_POINTS, 0, NUM_PARTICLES);
    CHECK_GL_ERROR("Draw")

    // Check elapsed time
    auto now = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> delta_time = now - last_time_s;
    last_time_s = now;
    float dts = delta_time.count();

    // Update the physics
    update_world(dts);

    // End of frame
    window.SwapBuffers();
  }
  return 0;
}

