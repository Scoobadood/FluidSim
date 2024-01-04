//
// Created by Dave Durbin on 2/1/2024.
//
#include "height_field_sim.h"
#include <cmath>

const float G = 9.8f;

HeightField::HeightField(uint32_t num_x_cols, uint32_t num_z_cols, float dim_x_m, float dim_z_m, float depth) //
        : wave_speed_level_{2}//
        , num_x_cols_{num_x_cols}//
        , num_z_cols_{num_z_cols} //

{
  col_spacing_x_ = dim_x_m / (float) num_x_cols;
  col_spacing_z_ = dim_z_m / (float) num_z_cols;
  heights_.resize(num_x_cols * num_z_cols, 1.0f);
  velocities_.resize(num_x_cols * num_z_cols, 0.0f);

  // From https://spark.iop.org/speed-water-waves
  // Speed of waves in shallow water approx sqrt(gh) where h is depth
  wave_propagation_speed_levels_.resize(5);
  wave_propagation_speed_levels_[2] = std::sqrtf(G * depth);
  wave_propagation_speed_levels_[1] = std::sqrtf(G * depth*2);
  wave_propagation_speed_levels_[0] = std::sqrtf(G * depth*4);
  wave_propagation_speed_levels_[3] = std::sqrtf(G * depth*0.8f);
  wave_propagation_speed_levels_[4] = std::sqrtf(G * depth*0.5f);
}

void HeightField::Init(InitMode mode) {
  heights_.clear();

  if (PULSE == mode) {
    float step_x = num_x_cols_ / 8.0f;
    float step_z = num_z_cols_ / 8.0f;
    for (auto z = 0; z < num_z_cols_; ++z) {
      for (auto x = 0; x < num_x_cols_; ++x) {
        float adj_x = (x / step_x) - 2.0f;
        float adj_z = (z / step_z) - 2.0f;
        auto height = std::expf(-(adj_x * adj_x) - (adj_z * adj_z));
        height = 2 + std::fmax(height, 0.0f) * 3;
        heights_.push_back(height);
      }
    }
  } else if (WAVE == mode) {
    float step_x = num_x_cols_ / 8.0f;
    for (auto z = 0; z < num_z_cols_; ++z) {
      for (auto x = 0; x < num_x_cols_; ++x) {
        float adj_x = (x / step_x);
        auto height = std::expf(-(adj_x * adj_x) - 0.5);
        height = 2.0f + std::fmax(height, 0.0f) * 3;
        heights_.push_back(height);
      }
    }
  } else {
    for (auto z = 0; z < num_z_cols_; ++z) {
      for (auto x = 0; x < num_x_cols_; ++x) {
        auto height = 2.0f +
                      (((x > num_x_cols_ * .25f) &&
                        (x < num_x_cols_ * .75f) &&
                        (z > num_z_cols_ * .25f) &&
                        (z < num_z_cols_ * .75f)) ? 3.0f : 0.0f);

        heights_.push_back(height);
      }
    }
  }

  std::fill(velocities_.begin(), velocities_.end(), 0.0f);
}

uint32_t HeightField::DimX() const { return num_x_cols_; }

uint32_t HeightField::DimZ() const { return num_z_cols_; }

const std::vector<float> &HeightField::Heights() const {
  return heights_;
}

void HeightField::Simulate(float delta_t) {
  auto idx = 0;
  auto c2 = wave_propagation_speed_levels_[wave_speed_level_] * wave_propagation_speed_levels_[wave_speed_level_];
  auto h2 = col_spacing_x_ * col_spacing_z_;
  for (auto z = 0; z < num_z_cols_; ++z) {
    for (auto x = 0; x < num_x_cols_; ++x) {
      auto left = (x > 0) ? heights_[idx - 1] : heights_[idx];
      auto right = (x < num_x_cols_ - 1) ? heights_[idx + 1] : heights_[idx];
      auto up = (z > 0) ? heights_[idx - num_x_cols_] : heights_[idx];
      auto down = (z < num_z_cols_ - 1) ? heights_[idx + num_x_cols_] : heights_[idx];

      velocities_[idx] += (delta_t * c2 * ((left + right + up + down) - (4 * heights_[idx])) / h2);
      velocities_[idx] *= 0.999f;
      idx++;
    }
  }
  for (auto i = 0; i < heights_.size(); ++i) {
    heights_[i] += (velocities_[i] * delta_t);
  }
}

void HeightField::IncreaseWaveSpeed() {
  if (wave_speed_level_ == 4) return;
  ++wave_speed_level_;
}

void HeightField::DecreaseWaveSpeed() {
  if (wave_speed_level_ == 0) return;
  --wave_speed_level_;
}
