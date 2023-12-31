#ifndef GRID_FLUID_RGB_SIMULATOR_H
#define GRID_FLUID_RGB_SIMULATOR_H

#include "grid_fluid_simulator.h"

#include <cstdint>
#include <vector>

class GridFluidRGBSimulator : public GridFluidSimulator {
public:
  GridFluidRGBSimulator(uint32_t width,      //
                        uint32_t height,     //
                        float delta_t,       //
                        float diffusion_rate //
  );

  [[nodiscard]] const std::vector<float> &DensityGreen() const;

  [[nodiscard]] const std::vector<float> &DensityBlue() const;

  void Simulate() override;

  void InitialiseDensity();

  void InitialiseVelocity();
private:

  std::vector<float> density_g_;
  std::vector<float> density_b_;
  std::vector<float> source_r_;
  std::vector<float> source_g_;
  std::vector<float> source_b_;
};

#endif // GRID_FLUID_RGB_SIMULATOR_H
