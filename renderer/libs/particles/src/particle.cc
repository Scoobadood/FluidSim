//
// Created by Dave Durbin on 5/1/2024.
//

#include "particle.h"
#include "spdlog/spdlog.h"

Particle::Particle(const glm::vec3 &position,
                   const glm::vec3 &colour,
                   float mass
) //
        : position_{position}//
        , colour_{colour} //
        , velocity_{}//
        , force_{}//
{
  if (mass <= 0) {
    spdlog::warn("Invalid mass {}, using infinite mass");
    inv_mass_ = 0.0f;
  } else {
    inv_mass_ = 1.0f / mass;
  }
}

void Particle::SetMass(float mass){
  if (mass <= 0) {
    spdlog::warn("Invalid mass {}, using infinite mass");
    inv_mass_ = 0.0f;
  } else {
    inv_mass_ = 1.0f / mass;
  }
}


void Particle::ApplyForce(const glm::vec3 &force) {
  force_ += force;
}

void Particle::ClearForce() {
  force_ = {0, 0, 0};
}

glm::vec3 Particle::Acceleration() const { return force_ * inv_mass_; }

const glm::vec3 &Particle::Position() const { return position_; }

const glm::vec3 &Particle::Velocity() const { return velocity_; }

void Particle::SetPosition(const glm::vec3 &position) { position_ = position; }

void Particle::SetVelocity(const glm::vec3 &velocity) { velocity_ = velocity; }

const glm::vec3 &Particle::Colour() const { return colour_; }
