#include "physics_props.h"
#include "glm/ext/matrix_transform.hpp"
#include "spdlog/spdlog.h"

PhysProperties::PhysProperties()//
        : position_{0, 0, 0}//
        , velocity_{0, 0, 0}//
        , force_{0, 0, 0}//
        , mass_kg_{1}//
        , orientation_{}//
        , inertial_tensor_{1}//
{
}

void PhysProperties::ApplyGravity() {
  force_ += (mass_kg_ * glm::vec3(0.0, -9.8f, 0));
}

void PhysProperties::ApplyForce(const glm::vec3 &p, const glm::vec3 &f) {
  auto r = p - centre_of_mass_;
  auto torque = glm::cross(r, f);
  // Right now, just apply the force as is
  force_ += f;
}

void PhysProperties::Tick(float delta_t) {
  glm::vec3 acceleration = force_ / mass_kg_;
  velocity_ += acceleration * delta_t;
  position_ += velocity_ * delta_t;

//  float angularAcceleration = rigidBody->torque / rigidBody->shape.momentOfInertia;
//  rigidBody->angularVelocity += angularAcceleration * dt;
//  rigidBody->angle += rigidBody->angularVelocity * dt;
  force_ = {0, 0, 0};
}

glm::mat4 PhysProperties::ModelToWorldMatrix() {
  glm::mat4 model{1.0f};
  model = glm::translate(model, position_);
  return model;
}

void PhysProperties::SetPosition(float x, float y, float z) {
  position_.x = x;
  position_.y = y;
  position_.z = z;
}

void PhysProperties::SetPosition(const glm::vec3 &position) {
  position_ = position;
}

void PhysProperties::SetVelocity(const glm::vec3 &velocity) {
  velocity_ = velocity;
}


void PhysProperties::SetBoundingBox(float min_x, float min_y, float min_z, float max_x, float max_y, float max_z) {
  bb_min_vertex_.x = min_x;
  bb_min_vertex_.y = min_y;
  bb_min_vertex_.z = min_z;
  bb_max_vertex_.x = max_x;
  bb_max_vertex_.y = max_y;
  bb_max_vertex_.z = max_z;
}

void PhysProperties::SetCentreOfMass(float c_x, float c_y, float c_z) {
  centre_of_mass_.x = c_x;
  centre_of_mass_.y = c_y;
  centre_of_mass_.z = c_z;
}
