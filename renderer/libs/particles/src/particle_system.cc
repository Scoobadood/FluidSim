#include "particle_system.h"
#include <spdlog/spdlog.h>
#include <random>

glm::vec3 random_up_vel() {
  static unsigned int seed = 123;
  static std::mt19937_64 rng{seed};
  static std::uniform_real_distribution<float> xz{-3, 3};
  static std::uniform_real_distribution<float> y{20, 30};
  return {xz(rng), y(rng), xz(rng)};
}

ParticleSystem::ParticleSystem(uint32_t num_particles, std::shared_ptr<ParticleFactory> &factory)//
    : factory_(factory)//
{
  for (auto i = 0; i < num_particles; ++i) {
    particles_.push_back(factory_->MakeParticle());
  }
}

const std::vector<std::shared_ptr<Particle>> &
ParticleSystem::Particles() const {
  return particles_;
}

void ParticleSystem::AddForceHandler(const std::shared_ptr<ForceHandler> &fh) {
  force_handlers_.push_back(fh);
}

void ParticleSystem::RemoveForceHandler(const std::shared_ptr<ForceHandler> &fh) {
  for (auto iter = force_handlers_.begin();
       iter != force_handlers_.end();) {
    if (iter->get() == fh.get()) {
      iter = force_handlers_.erase(iter);
    } else {
      ++iter;
    }
  }
}

void ParticleSystem::Constrain() {
  for (auto &p : particles_) {
    if (p->Position().y < 0) {
      // Respawn. But with same velocity
      p->SetPosition({0, 0, 0});
      p->SetVelocity(random_up_vel());
      break;
    }
  }
}

std::vector<float> ParticleSystem::GetState() const {
  std::vector<float> state;
  state.reserve(particles_.size() * 6);
  for (const auto &p : particles_) {
    state.insert(state.end(), {p->Position().x, p->Position().y, p->Position().z});
    state.insert(state.end(), {p->Velocity().x, p->Velocity().y, p->Velocity().z});
  }
  return state;
}

void ParticleSystem::SetState(const std::vector<float> &state) {
  auto idx = 0;
  for (auto &p : particles_) {
    p->SetPosition({state[idx], state[idx + 1], state[idx + 2]});
    p->SetVelocity({state[idx + 3], state[idx + 4], state[idx + 5]});
    idx += 6;
  }
}

void ParticleSystem::ClearForces() {
  for (auto &p : particles_) p->ClearForce();
}

void ParticleSystem::ComputeForces() {
  for (auto &fh : force_handlers_) {
    fh->Apply(*this);
  }
}

std::vector<float> ParticleSystem::Derivative() {
  ClearForces();   /* zero the force accumulators */
  ComputeForces(); /* magic force function */
  std::vector<float> derivative;
  derivative.reserve(particles_.size() * 6);
  for (const auto &p : particles_) {
    // Derivatibve of X is V
    derivative.insert(derivative.end(), {p->Velocity().x, p->Velocity().y, p->Velocity().z});
    // Derivative of V is A (F/m)
    derivative.insert(derivative.end(), {p->Acceleration().x, p->Acceleration().y, p->Acceleration().z});
  }
  for (auto &d : derivative) {
    if (isnan(d)) {
      spdlog::critical("NaN");
    }
  }
  return derivative;

}

std::vector<float> ParticleSystem::Positions() {
  std::vector<float> pos;
  pos.reserve(particles_.size() * 3);
  for (const auto &p : particles_) {
    pos.insert(pos.end(), {p->Position().x, p->Position().y, p->Position().z});
  }
  return pos;
}

void ParticleSystem::CreateDragParticleAt(int32_t target_idx, const glm::vec3 &pos) {
  dragee_index_ = target_idx;
  drag_particle_ = std::make_shared<Particle>(pos, glm::vec3{1,1,1},-1);
  auto len = glm::length(particles_[target_idx]->Position() - pos);
  drag_spring_= std::make_shared<SpringForceHandler>(particles_[target_idx],
                                       drag_particle_,
                                       len,
                                       10,
                                       1);
  AddForceHandler(drag_spring_);
}

void ParticleSystem::StopDragging() {
  RemoveForceHandler(drag_spring_);
  drag_particle_ = nullptr;
  dragee_index_ = -1;
}