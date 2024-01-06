#include "force_handlers.h"
#include "particle_system.h"
#include "particle.h"

GlobalForceHandler::GlobalForceHandler(const glm::vec3 &force)//
        : force_{force}//
{}

void GlobalForceHandler::Apply(ParticleSystem &particle_system) {
  for (auto &p: particle_system.Particles()) {
    p->ApplyForce(force_);
  }
}

SpringForceHandler::SpringForceHandler(const std::shared_ptr<Particle> &p1,
                                       const std::shared_ptr<Particle> &p2,
                                       float rest_length,
                                       float spring_constant,
                                       float damping_constant)//
        : p1_{p1}//
        , p2_{p2}//;
        , rest_length_{rest_length}//
        , spring_constant_{spring_constant}//
        , damping_constant_{damping_constant}//
{}

/*
 * Hook spring force
 * f_a = -[ ks( |L| - rest_length) + k_d( dot(V, L)/|L|)] L_unit
 * L = P_a = P_b
 * V = V_a - V_b
 */
void SpringForceHandler::Apply(ParticleSystem &particle_system) {
  auto l = p1_->Position() - p2_->Position();
  auto len_l = glm::length(l);
  auto ln = l / len_l;
  auto v = p1_->Velocity() - p2_->Velocity();
  auto t1 = spring_constant_ * (len_l - rest_length_);
  auto t2 = damping_constant_ * glm::dot(v, l) / len_l;
  auto fa = -(t1 + t2) * ln;
  auto fb = -fa;
  p1_->ApplyForce(fa);
  p2_->ApplyForce(fb);
}

ClickForceHandler::ClickForceHandler(const std::shared_ptr<Particle> &particle, glm::vec3 force)//
        : particle_{particle}//
        , force_{force}//
{
  is_ready_ = false;
  time_out_ = 0;
}

void ClickForceHandler::Trigger() {
  if (is_ready_) return;
  if (time_out_ > 0) return;
  is_ready_ = true;
}

void ClickForceHandler::Apply(ParticleSystem &particle_system) {
  if (is_ready_) {
    particle_->ApplyForce(force_);
    is_ready_ = false;
    time_out_ = 5000;
    return;
  }
  if (time_out_ > 0) time_out_--;
}
