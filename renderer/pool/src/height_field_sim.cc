//
// Created by Dave Durbin on 2/1/2024.
//
#include "height_field_sim.h"

HeightField::HeightField(uint32_t dim_x, uint32_t dim_z) //
        : dim_x_{dim_x}//
        , dim_z_{dim_z} //
{
  heights_.resize(dim_x * dim_z, 1.0f);
  velocities_.resize(dim_x * dim_z, 0.0f);
}

void HeightField::Init() {
  heights_.clear();
  auto mid_x = (float) dim_x_ / 2.0f;
  auto mid_z = (float) dim_z_ / 2.0f;
  for (auto z = 0; z < dim_z_; ++z) {
    for (auto x = 0; x < dim_x_; ++x) {
      auto hz = 1.0f - (std::abs((float) z - mid_z)) / (float) dim_z_;
      auto hx = 1.0f - (std::abs((float) x - mid_x)) / (float) dim_x_;
      heights_.push_back(hx + hz);
    }
  }
  std::fill(velocities_.begin(), velocities_.end(), 0.0f);
}

uint32_t HeightField::DimX() const { return dim_x_; }

uint32_t HeightField::DimZ() const { return dim_z_; }

const std::vector<float> &HeightField::Heights() const {
  return heights_;
}

void HeightField::Simulate() {
  auto idx = 0;
  for (auto z = 0; z < dim_z_; ++z) {
    for (auto x = 0; x < dim_x_; ++x) {
      auto left = (x > 0) ? heights_[idx - 1] : heights_[idx];
      auto right = (x < dim_x_ - 1) ? heights_[idx + 1] : heights_[idx];
      auto up = (z > 0) ? heights_[idx - dim_x_] : heights_[idx];
      auto down = (z < dim_z_ - 1) ? heights_[idx + dim_x_] : heights_[idx];

      velocities_[idx] += ((left + right + up + down) * 0.25f - heights_[idx]);
      velocities_[idx] *= 0.995f;

      idx++;
    }
  }
  for (auto i = 0; i < heights_.size(); ++i) {
    heights_[i] += velocities_[i];
  }
}
