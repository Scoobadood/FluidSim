//
// Created by Dave Durbin on 5/1/2024.
//

#ifndef FLUIDSIM_FORCE_HANDLERS_H
#define FLUIDSIM_FORCE_HANDLERS_H

#include <memory>
#include "glm/vec3.hpp"
#include "particle.h"

class ParticleSystem;

class ForceHandler {
public:
  virtual ~ForceHandler() = default;

  virtual void Apply(ParticleSystem &particle_system) = 0;
};

class ViscousDragHandler : public ForceHandler {
public:
  explicit ViscousDragHandler(float drag_coefficient);

  ~ViscousDragHandler() override = default;

  void Apply(ParticleSystem &particle_system) override;

private :
  float drag_coefficient_;
};

class GlobalForceHandler : public ForceHandler {
public:
  explicit GlobalForceHandler(const glm::vec3 &force);

  ~GlobalForceHandler() override = default;

  void Apply(ParticleSystem &particle_system) override;

private :
  glm::vec3 force_;
};

class SpringForceHandler : public ForceHandler {
public:
  SpringForceHandler(const std::shared_ptr<Particle> &p1,
                     const std::shared_ptr<Particle> &p2,
                     float rest_length,
                     float spring_constant,
                     float damping_constant
  );

  ~SpringForceHandler() override = default;

  void Apply(ParticleSystem &particle_system) override;

private :
  std::shared_ptr<Particle> p1_;
  std::shared_ptr<Particle> p2_;
  float rest_length_;
  float spring_constant_;
  float damping_constant_;
};


class ClickForceHandler : public ForceHandler {
public:
  ClickForceHandler(const std::shared_ptr<Particle> &particle, glm::vec3 force);

  ~ClickForceHandler() override = default;

  void Trigger();

  void Apply(ParticleSystem &particle_system) override;

private:
  std::shared_ptr<Particle> particle_;
  glm::vec3 force_;
  float time_out_;
  bool is_ready_;
};

#endif //FLUIDSIM_FORCE_HANDLERS_H
