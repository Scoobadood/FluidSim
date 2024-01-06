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

  auto diff_pos = p1_->Position() - p2_->Position();
  auto diff_vel = p1_->Velocity() - p2_->Velocity();

  auto spring_force_magnitude = spring_constant_ * (glm::length(diff_pos) - rest_length_);
  auto damping_force_magnitude = damping_constant_ * (glm::dot(diff_pos, diff_vel) / glm::length(diff_pos));
  auto unit_dir = glm::normalize(diff_pos);
  auto fa = -(spring_force_magnitude + damping_force_magnitude) * unit_dir;
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

ViscousDragHandler::ViscousDragHandler(float drag_coefficient)//
        : drag_coefficient_{drag_coefficient}//
{}

void ViscousDragHandler::Apply(ParticleSystem &particle_system) {
  for (auto &p: particle_system.Particles()) {
    p->ApplyForce(p->Velocity() * -drag_coefficient_);
  }
}