//
// Created by Dave Durbin on 5/1/2024.
//

#ifndef FLUIDSIM_PARTICLE_H
#define FLUIDSIM_PARTICLE_H

#include <glm/glm.hpp>

class Particle {
public:
  explicit Particle(const glm::vec3 &position,
                    const glm::vec3 &colour,
                    float mass = 1.0f
  );

  void ApplyForce(const glm::vec3 &force);

  void ClearForce();

  const glm::vec3 &Position() const;

  void SetPosition(const glm::vec3 &position);

  const glm::vec3 &Velocity() const;

  void SetVelocity(const glm::vec3 &velocity);

  glm::vec3 Acceleration() const;

  const glm::vec3 &Colour() const;

private:
  glm::vec3 position_;
  glm::vec3 colour_;
  glm::vec3 velocity_;
  glm::vec3 force_;
  float inv_mass_;
};

#endif //FLUIDSIM_PARTICLE_H
