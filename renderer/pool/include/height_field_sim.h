//
// Created by Dave Durbin on 2/1/2024.
//

#ifndef FLUIDSIM_HEIGHT_FIELD_SIM_H
#define FLUIDSIM_HEIGHT_FIELD_SIM_H

#include <vector>

class HeightField {
public:
  HeightField(uint32_t dim_x, uint32_t dim_z);
  void Init();
  uint32_t DimX() const;
  uint32_t DimZ() const;
  const std::vector<float>& Heights() const;
  void Simulate();

private:
  uint32_t dim_x_;
  uint32_t dim_z_;
  std::vector<float> heights_;
  std::vector<float> velocities_;
};
#endif //FLUIDSIM_HEIGHT_FIELD_SIM_H
