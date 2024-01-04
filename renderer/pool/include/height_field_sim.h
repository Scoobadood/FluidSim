//
// Created by Dave Durbin on 2/1/2024.
//

#ifndef FLUIDSIM_HEIGHT_FIELD_SIM_H
#define FLUIDSIM_HEIGHT_FIELD_SIM_H

#include <vector>

class HeightField {
public:
  enum InitMode {
    PULSE,
    WAVE,
    CUBE
  };
  HeightField(uint32_t num_x_cols, uint32_t num_z_cols, float dim_x_m, float dim_z_m, float depth);
  void Init(InitMode mode);
  uint32_t DimX() const;
  uint32_t DimZ() const;
  const std::vector<float>& Heights() const;
  void IncreaseWaveSpeed();
  void DecreaseWaveSpeed();
  void Simulate(float delta_t);

private:
  uint32_t wave_speed_level_;
  std::vector<float> wave_propagation_speed_levels_;
  uint32_t num_x_cols_;
  uint32_t num_z_cols_;
  float col_spacing_x_;
  float col_spacing_z_;
  std::vector<float> heights_;
  std::vector<float> velocities_;
};
#endif //FLUIDSIM_HEIGHT_FIELD_SIM_H
