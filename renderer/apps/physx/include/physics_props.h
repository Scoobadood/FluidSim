//
// Created by Dave Durbin on 4/1/2024.
//

#ifndef FLUIDSIM_PHYSICS_PROPS_H
#define FLUIDSIM_PHYSICS_PROPS_H

#include "glm/vec3.hpp"
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/quaternion_float.hpp"

class PhysProperties {
public:
  PhysProperties();

  glm::mat4 ModelToWorldMatrix();

  void Tick(float delta_t);

  void ApplyGravity();

  void ApplyForce(const glm::vec3 &p, const glm::vec3 &f);

  const glm::vec3 &Position() const { return position_; }

  void SetPosition(float x, float y, float z);

  void SetPosition(const glm::vec3 &position);

  void SetVelocity(const glm::vec3 &velocity);

  void SetBoundingBox(float min_x, float min_y, float min_z, float max_x, float max_y, float max_z);

  const glm::vec3 &AABBMinVertex() const { return bb_min_vertex_; }

  const glm::vec3 &AABBMaxVertex() const { return bb_max_vertex_; }

  void SetCentreOfMass(float c_x, float c_y, float c_z);

  const glm::vec3 &CentreOfMass() const { return centre_of_mass_; }

private:
  glm::vec3 position_;
  glm::vec3 velocity_;
  glm::vec3 force_;
  float mass_kg_;
  glm::quat orientation_;
  glm::mat3 inertial_tensor_;
  glm::vec3 centre_of_mass_;
  glm::vec3 bb_min_vertex_;
  glm::vec3 bb_max_vertex_;
};

#endif //FLUIDSIM_PHYSICS_PROPS_H
