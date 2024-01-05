#ifndef FLUIDSIM_PARTICLE_SYSTEM_H
#define FLUIDSIM_PARTICLE_SYSTEM_H

#include <glm/glm.hpp>
#include <vector>

class Particle {
public:
  explicit Particle(const glm::vec3 &position,
                    const glm::vec3 &colour,
                    float mass = 1.0f
  );

  void ApplyForce(const glm::vec3 &force);
  void ClearForce();

  void Resolve(float delta_t);

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

class ParticleFactory {
public:
  static std::shared_ptr<Particle> MakeParticle();
};

class ParticleSystem {
public:
  ParticleSystem(uint32_t num_particles, ParticleFactory factory);

  const std::vector<std::shared_ptr<Particle>> &Particles() const;

  /* Returns X and V for each particle as a single vector of floats*/
  std::vector<float> GetState() const;

  void SetState(const std::vector<float> &state);

  std::vector<float> Derivative();

  inline uint32_t NumParticles() const { return particles_.size(); }

  void Constrain();

protected:
  void ClearForces();

  void ComputeForces();

private:
  std::vector<std::shared_ptr<Particle>> particles_;
  ParticleFactory factory_;

};

void particles_calculate_forces(ParticleSystem &ps);

#endif //FLUIDSIM_PARTICLE_SYSTEM_H
