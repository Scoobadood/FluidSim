//
// Created by Dave Durbin on 3/1/2024.
//
#include "renderer.h"

#include "GLHelpers/GLHelpers.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"

Renderer::Renderer(uint32_t width, uint32_t height)//
    : projection_{1}//
    , frame_buffer_width_{(GLsizei) width}//
    , frame_buffer_height_{(GLsizei) height}//
{
}

void Renderer::InitGL() {
    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);

    GLuint buff[2];
    glGenBuffers(2, buff);
    vbo_ = buff[0];
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 24, (GLvoid *) nullptr);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 24, (GLvoid *) 12);

    ebo_ = buff[1];
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);

    CHECK_GL_ERROR("Alloc buffers")
}

void Renderer::SetFrameBufferSize(uint32_t width, uint32_t height) {
  frame_buffer_width_ = (GLsizei) width;
  frame_buffer_height_ = (GLsizei) height;
  auto ratio = (float) frame_buffer_width_ / (float) frame_buffer_height_;
  projection_ = glm::perspective(glm::radians(35.0f), ratio, 0.1f, 1000.0f);

  if (shader_) {
    shader_->use();
    shader_->set_uniform("project", projection_);
  }
}

void Renderer::InitShaders() {
  shader_ = Shader::from_files(//
      "/Users/dave/Projects/FluidSim/renderer/apps/party/shaders/basic.vert",
      "/Users/dave/Projects/FluidSim/renderer/apps/party/shaders/basic.frag");
  CHECK_GL_ERROR("Create shader")

  InitGL();
}

void Renderer::Render(const std::shared_ptr<ParticleSystem> &particle_system,
                      const glm::mat4& cam_rot) {
  glViewport(0, 0, frame_buffer_width_, frame_buffer_height_);

  glEnable(GL_DEPTH_TEST);
  glClearColor(0.2, 0.2, 0.2, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  shader_->use();

  // View Things
  glm::mat4 view = glm::mat4{1};
  view = glm::translate(view, -camera_position_);
  view = view * cam_rot;
  shader_->set_uniform("view", view);
  shader_->set_uniform("model", glm::mat4(1));

  // Draw particles
  std::vector<float> particle_data;
  for (const auto &p : particle_system->Particles()) {
    particle_data.insert(particle_data.end(), {p->Position().x, p->Position().y, p->Position().z});
    particle_data.insert(particle_data.end(), {p->Colour().r, p->Colour().g, p->Colour().b});
  }
  glBufferData(GL_ARRAY_BUFFER,
               (GLsizeiptr) (particle_data.size() * sizeof(float)),
               particle_data.data(),
               GL_DYNAMIC_DRAW);
  uint32_t lines[] = {0, 1, 1, 2, 2, 3, 3, 0, 0, 4, 1, 4, 2, 4, 3, 4};
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               8 * 2 * sizeof(uint32_t),
               lines,
               GL_DYNAMIC_DRAW);

  glDrawArrays(GL_POINTS, 0, particle_system->NumParticles());
  glDrawElements(GL_LINES, 16 * 2, GL_UNSIGNED_INT, nullptr);
  CHECK_GL_ERROR("Draw")

  // Draw a surroundy box if needed
  auto selected_index = particle_system->SelectedIndex();
  if (selected_index >= 0) {
    float box_data[4 * 6];
    auto px = particle_data.at(selected_index * 6);
    auto py = particle_data.at(selected_index * 6 + 1);
    auto pz = particle_data.at(selected_index * 6 + 2);
    auto pr = particle_data.at(selected_index * 6 + 3);
    auto pg = particle_data.at(selected_index * 6 + 4);
    auto pb = particle_data.at(selected_index * 6 + 5);
    // Colour all verts
    for (auto v = 0; v < 4; v++) {
      box_data[v * 6 + 0 + 2] = pz;
      box_data[v * 6 + 3 + 0] = pr;
      box_data[v * 6 + 3 + 1] = pg;
      box_data[v * 6 + 3 + 2] = pb;
    }
    box_data[0 * 6 + 0 + 0] = px - 0.1f;
    box_data[0 * 6 + 0 + 1] = py - 0.1f;

    box_data[1 * 6 + 0 + 0] = px + 0.1f;
    box_data[1 * 6 + 0 + 1] = py - 0.1f;

    box_data[2 * 6 + 0 + 0] = px + 0.1f;
    box_data[2 * 6 + 0 + 1] = py + 0.1f;

    box_data[3 * 6 + 0 + 0] = px - 0.1f;
    box_data[3 * 6 + 0 + 1] = py + 0.1f;

    glBufferData(GL_ARRAY_BUFFER,
                 (GLsizeiptr) (24 * sizeof(float)), box_data, GL_DYNAMIC_DRAW);
    glDrawElements(GL_LINES, 8, GL_UNSIGNED_INT, nullptr);
  }
}