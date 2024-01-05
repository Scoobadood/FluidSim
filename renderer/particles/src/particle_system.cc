#include "particle_system.h"
#include <spdlog/spdlog.h>
#include <random>

Particle::Particle(const glm::vec3 &position,
                   const glm::vec3 &colour,
                   float mass
) //
        : position_{position}//
        , colour_{colour} //
{
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

void Particle::Resolve(float delta_t) {
  auto accel = force_ * inv_mass_;
  velocity_ += (delta_t * accel);
  position_ += (delta_t * velocity_);
  force_ = {0, 0, 0};
}

const glm::vec3 &Particle::Position() const { return position_; };

void Particle::SetPosition(const glm::vec3 &position) { position_ = position; };

const glm::vec3 &Particle::Colour() const { return colour_; };


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

std::shared_ptr<Particle> ParticleFactory::MakeParticle() const {
  auto p = random_position();
  auto c = random_colour();
  return std::make_shared<Particle>(p, c);
}

ParticleSystem::ParticleSystem(uint32_t num_particles, ParticleFactory factory)
        : factory_(factory)//
{
  for (auto i = 0; i < num_particles; ++i) {
    particles_.push_back(factory_.MakeParticle());
  }
}


const std::vector<std::shared_ptr<Particle>> &
ParticleSystem::Particles() const {
  return particles_;
}

void ParticleSystem::Update(float delta_t) {
  for (auto p: particles_) {
    p->ApplyForce({0, -9.8f, 0});
    p->Resolve(delta_t);
    if (p->Position().y < -150) {
      p->SetPosition(random_position());
    }
  }
}
