#include "particle_system.h"
#include <spdlog/spdlog.h>
#include <random>

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

void Particle::ApplyForce(const glm::vec3 &force) {
  force_ += force;
}

void Particle::ClearForce() {
  force_ = {0, 0, 0};
}

void Particle::Resolve(float delta_t) {
  auto accel = force_ * inv_mass_;
  velocity_ += (delta_t * accel);
  position_ += (delta_t * velocity_);
  force_ = {0, 0, 0};
}

glm::vec3 Particle::Acceleration() const { return force_ * inv_mass_; }

const glm::vec3 &Particle::Position() const { return position_; }

const glm::vec3 &Particle::Velocity() const { return velocity_; }

void Particle::SetPosition(const glm::vec3 &position) { position_ = position; }

void Particle::SetVelocity(const glm::vec3 &velocity) { velocity_ = velocity; }

const glm::vec3 &Particle::Colour() const { return colour_; }


glm::vec3 random_position() {
  static unsigned int seed = 123;
  static std::mt19937_64 rng{seed};
  static std::uniform_real_distribution<float> r{0, 10};
  return {r(rng), r(rng), r(rng)};
}
glm::vec3 random_up_vel() {
  static unsigned int seed = 123;
  static std::mt19937_64 rng{seed};
  static std::uniform_real_distribution<float> xz{-3, 3};
  static std::uniform_real_distribution<float> y{20, 30};
  return {xz(rng), y(rng), xz(rng)};
}

glm::vec3 random_colour() {
  static unsigned int seed = 123;
  static std::mt19937_64 rng{seed};
  static std::uniform_real_distribution<float> col{0, 1.0};
  return {col(rng), col(rng), col(rng)};
}

std::shared_ptr<Particle> ParticleFactory::MakeParticle() {
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

void ParticleSystem::Constrain() {
  for (auto &p: particles_) {
      if (p->Position().y<0){
        // Respawn. But with same velocity
        p->SetPosition({0,0,0});
        p->SetVelocity(random_up_vel());
        break;
    }
  }
}

std::vector<float> ParticleSystem::GetState() const {
  std::vector<float> state;
  state.reserve(particles_.size() * 6);
  for (const auto &p: particles_) {
    state.insert(state.end(), {p->Position().x, p->Position().y, p->Position().z});
    state.insert(state.end(), {p->Velocity().x, p->Velocity().y, p->Velocity().z});
  }
  return state;
}

void ParticleSystem::SetState(const std::vector<float> &state) {
  auto idx = 0;
  for (auto &p: particles_) {
    p->SetPosition({state[idx], state[idx + 1], state[idx + 2]});
    p->SetVelocity({state[idx + 3], state[idx + 4], state[idx + 5]});
    idx += 6;
  }
}

void ParticleSystem::ClearForces() {
  for (auto &p: particles_) p->ClearForce();
}

void ParticleSystem::ComputeForces() {
  for (const auto &p: particles_) {
    p->ApplyForce({0, -9.8f, 0});
  }
}

std::vector<float> ParticleSystem::Derivative() {
  ClearForces();   /* zero the force accumulators */
  ComputeForces(); /* magic force function */
  std::vector<float> derivative;
  derivative.reserve(particles_.size() * 6);
  for (const auto &p: particles_) {
    // Derivatibve of X is V
    derivative.insert(derivative.end(), {p->Velocity().x, p->Velocity().y, p->Velocity().z});
    // Derivative of V is A (F/m)
    derivative.insert(derivative.end(), {p->Acceleration().x, p->Acceleration().y, p->Acceleration().z});
  }
  return derivative;

}