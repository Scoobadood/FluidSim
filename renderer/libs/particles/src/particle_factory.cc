#include "particle_factory.h"
#include "particle.h"

#include <random>

glm::vec3 random_position() {
  static unsigned int seed = 123;
  static std::mt19937_64 rng{seed};
  static std::uniform_real_distribution<float> r{0, 10};
  return {r(rng), r(rng), r(rng)};
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


GridParticleFactory::GridParticleFactory(uint32_t dim_x, uint32_t dim_y, uint32_t dim_z, float spacing)//
        : ParticleFactory{}//
        , dim_x_{dim_x}//
        , dim_y_{dim_y}//
        , dim_z_{dim_z}//
        , spacing_{spacing}//
{
  min_x_ = -spacing_ * (dim_x / 2.0f);
  min_y_ = -spacing_ * (dim_y / 2.0f);
  min_z_ = -spacing_ * (dim_z / 2.0f);
  curr_x_ = 0;
  curr_y_ = 0;
  curr_z_ = 0;
}

std::shared_ptr<Particle> GridParticleFactory::MakeParticle() {
  auto pos = glm::vec3{min_x_ + spacing_ * (float) curr_x_,
                       min_y_ + spacing_ * (float) curr_y_,
                       min_z_ + spacing_ * (float) curr_z_};

  auto col = glm::vec3{(float) curr_x_ / (float) (dim_x_ - 1),
                       (float) curr_y_ / (float) (dim_y_ - 1),
                       (float) curr_z_ / (float) (dim_z_ - 1)};

  auto p = std::make_shared<Particle>(pos, col);
  if (++curr_x_ == dim_x_) {
    curr_x_ = 0;
    if (++curr_y_ == dim_y_) {
      curr_y_ = 0;
      if (++curr_z_ == dim_z_) {
        curr_z_ = 0;
      }
    }
  }

  return p;
}

