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

  void Resolve(float delta_t);

  const glm::vec3 &Position() const;

  void SetPosition(const glm::vec3 &position);

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
  std::shared_ptr<Particle> MakeParticle() const;
};

class ParticleSystem {
public:
  ParticleSystem(uint32_t num_particles, ParticleFactory factory);

  const std::vector<std::shared_ptr<Particle>> &Particles() const;

  void Update(float delta_t);

  inline uint32_t NumParticles() const { return particles_.size(); }

private:
  std::vector<std::shared_ptr<Particle>> particles_;
  ParticleFactory factory_;

};

#endif //FLUIDSIM_PARTICLE_SYSTEM_H
