//
// Created by Dave Durbin on 5/1/2024.
//

#ifndef FLUIDSIM_PARTICLE_FACTORY_H
#define FLUIDSIM_PARTICLE_FACTORY_H

#include <memory>
#include "particle.h"

class ParticleFactory {
public:
  virtual ~ParticleFactory() = default;
  virtual std::shared_ptr<Particle> MakeParticle();
};

class GridParticleFactory : public ParticleFactory {
public:
  GridParticleFactory(uint32_t dim_x, uint32_t dim_y, uint32_t dim_z, float spacing);
  ~GridParticleFactory() override = default;

  std::shared_ptr<Particle> MakeParticle() override;

private:
  uint32_t dim_x_;
  uint32_t dim_y_;
  uint32_t dim_z_;
  float spacing_;
  uint32_t curr_x_;
  uint32_t curr_y_;
  uint32_t curr_z_;
  float min_x_;
  float min_y_;
  float min_z_;
};


#endif //FLUIDSIM_PARTICLE_FACTORY_H
