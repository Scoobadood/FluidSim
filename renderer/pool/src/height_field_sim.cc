//
// Created by Dave Durbin on 2/1/2024.
//
#include "height_field_sim.h"
#include <cmath>

HeightField::HeightField(uint32_t dim_x, uint32_t dim_z) //
        : dim_x_{dim_x}//
        , dim_z_{dim_z} //
{
  heights_.resize(dim_x * dim_z, 1.0f);
  velocities_.resize(dim_x * dim_z, 0.0f);
}

void HeightField::Init(InitMode mode) {
  heights_.clear();

  if (PULSE == mode) {
    float step_x = dim_x_ / 8.0f;
    float step_z = dim_z_ / 8.0f;
    for (auto z = 0; z < dim_z_; ++z) {
      for (auto x = 0; x < dim_x_; ++x) {
        float adj_x = (x / step_x) - 2.0f;
        float adj_z = (z / step_z) - 2.0f;
        auto height = std::expf(-(adj_x * adj_x) - (adj_z * adj_z));
        height = 1.0f + std::fmax(height, 0.0f) * 2;
        heights_.push_back(height);
      }
    }
  } else if (WAVE == mode) {
    float step_x = dim_x_ / 8.0f;
    for (auto z = 0; z < dim_z_; ++z) {
      for (auto x = 0; x < dim_x_; ++x) {
        float adj_x = (x / step_x);
        auto height = std::expf(-(adj_x * adj_x)-0.5);
        height = 1.0f + std::fmax(height, 0.0f) * 2;
        heights_.push_back(height);
      }
    }
  } else {
    for (auto z = 0; z < dim_z_; ++z) {
      for (auto x = 0; x < dim_x_; ++x) {
        auto height = 1.0f +
                      (((x > dim_x_ * .25f) &&
                        (x < dim_x_ * .75f) &&
                        (z > dim_z_ * .25f) &&
                        (z < dim_z_ * .75f)) ? 1.0f : 0.0f);

        heights_.push_back(height);
      }
    }
  }

  std::fill(velocities_.begin(), velocities_.end(), 0.0f);
}

uint32_t HeightField::DimX() const { return dim_x_; }

uint32_t HeightField::DimZ() const { return dim_z_; }

const std::vector<float> &HeightField::Heights() const {
  return heights_;
}

void HeightField::Simulate(float delta_t) {
  auto idx = 0;
  auto c2 = 20.f*20.f;
  auto h2 = 1.f*1.f;
  for (auto z = 0; z < dim_z_; ++z) {
    for (auto x = 0; x < dim_x_; ++x) {
      auto left = (x > 0) ? heights_[idx - 1] : heights_[idx];
      auto right = (x < dim_x_ - 1) ? heights_[idx + 1] : heights_[idx];
      auto up = (z > 0) ? heights_[idx - dim_x_] : heights_[idx];
      auto down = (z < dim_z_ - 1) ? heights_[idx + dim_x_] : heights_[idx];

      velocities_[idx] += delta_t *c2 *((left + right + up + down) * 0.25f - heights_[idx])/h2;
      velocities_[idx] *=0.999f;
      idx++;
    }
  }
  for (auto i = 0; i < heights_.size(); ++i) {
    heights_[i] += velocities_[i] * delta_t;
  }
}
