//
// Created by Dave Durbin on 1/1/2024.
//

#ifndef FLUIDSIM_SCENE_H
#define FLUIDSIM_SCENE_H

#include "GLFW/glfw3.h"
#include "shader.h"

class Scene {
public:
  Scene();
  void Render();

private:
  GLuint vao_;
  GLuint vbo_;
  GLuint ebo_;
  GLsizei num_elements_;
  std::shared_ptr<Shader> shader_;
};

#endif //FLUIDSIM_SCENE_H
