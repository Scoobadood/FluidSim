#ifndef FLUIDSIM_PARTICLE_SYSTEM_H
#define FLUIDSIM_PARTICLE_SYSTEM_H

#include <glm/glm.hpp>
#include <vector>
#include "force_handlers.h"
#include "particle.h"
#include "particle_factory.h"

class ParticleSystem {
public:
  ParticleSystem(uint32_t num_particles, std::shared_ptr<ParticleFactory> &factory);

  const std::vector<std::shared_ptr<Particle>> &Particles() const;

  /* Returns X and V for each particle as a single vector of floats*/
  std::vector<float> GetState() const;

  void SetState(const std::vector<float> &state);

  std::vector<float> Positions();
  std::vector<float> Derivative();

  inline uint32_t NumParticles() const { return particles_.size(); }

  void AddForceHandler(const std::shared_ptr<ForceHandler> &fh);
  void RemoveForceHandler(const std::shared_ptr<ForceHandler> &fh);

  void Constrain();

  int32_t SelectedIndex() const{ return dragee_index_;}
  void CreateDragParticleAt(int32_t target_idx, const glm::vec3 &pos);
  void StopDragging();

 protected:
  void ClearForces();

  void ComputeForces();

private:
  std::vector<std::shared_ptr<Particle>> particles_;
  std::shared_ptr<ParticleFactory> factory_;
  std::vector<std::shared_ptr<ForceHandler>> force_handlers_;
  std::shared_ptr<Particle> drag_particle_;
  int32_t dragee_index_;
  std::shared_ptr<ForceHandler> drag_spring_;
};

#endif //FLUIDSIM_PARTICLE_SYSTEM_H
