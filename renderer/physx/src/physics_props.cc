#include "physics_props.h"
#include "glm/ext/matrix_transform.hpp"

PhysProperties::PhysProperties()//
        : position_{0, 0, 0}//
        , velocity_{0, 0, 0}//
        , force_{0, 0, 0}//
        , mass_kg_{1}//
{
}

void PhysProperties::ApplyGravity() {
  force_ += (mass_kg_ * glm::vec3(0.0, -9.8f, 0));
}

void PhysProperties::Tick(float delta_t) {
  glm::vec3 acceleration_ = force_ / mass_kg_;
  velocity_ += acceleration_ * delta_t;
  position_ += velocity_ * delta_t;

//  float angularAcceleration = rigidBody->torque / rigidBody->shape.momentOfInertia;
//  rigidBody->angularVelocity += angularAcceleration * dt;
//  rigidBody->angle += rigidBody->angularVelocity * dt;
  force_ = {0,0,0};
}

glm::mat4 PhysProperties::ModelToWorldMatrix() {
  glm::mat4 model{1.0f};
  model=glm::translate(model, position_);
  return model;
}

void PhysProperties::SetPosition(float x, float y, float z){
  position_.x = x;
  position_.y = y;
  position_.z = z;
}

