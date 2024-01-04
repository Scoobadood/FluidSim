//
// Created by Dave Durbin on 4/1/2024.
//

#ifndef FLUIDSIM_PHYSICS_PROPS_H
#define FLUIDSIM_PHYSICS_PROPS_H

#include "glm/vec3.hpp"
#include "glm/ext/matrix_float4x4.hpp"

class PhysProperties {
public:
  PhysProperties();

  glm::mat4 ModelToWorldMatrix();

  void Tick(float delta_t);

  void ApplyGravity();

  const glm::vec3 &Position() const { return position_; }
  void SetPosition(float x, float y, float z);

private:
  glm::vec3 position_;
  glm::vec3 velocity_;
  glm::vec3 force_;
  float mass_kg_;
};

#endif //FLUIDSIM_PHYSICS_PROPS_H
