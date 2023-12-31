#ifndef GRID_FLUID_SIMULATOR_H
#define GRID_FLUID_SIMULATOR_H

#include "fluid_simulator_2d.h"

#include <cstdint>
#include <vector>

class GridFluidSimulator : public FluidSimulator2D {
public:
  GridFluidSimulator(uint32_t width,      //
                     uint32_t height,     //
                     float delta_t,       //
                     float diffusion_rate //
  );

  void Simulate() override;

  void InitialiseDensity();

  void InitialiseVelocity();

protected:
  void Diffuse(const std::vector<float> &current_density, std::vector<float> &next_density);

  void SuppressDivergence();

private:
  [[nodiscard]] float AdvectValue(const std::vector<float> &velocity_x,
                    const std::vector<float> &velocity_y,
                    const std::vector<float> &source_data,
                    uint32_t x, uint32_t y) const;

  void AdvectDensity(const std::vector<float> &curr_density,
                     std::vector<float> &next_density) const;

  void AdvectVelocity(std::vector<float>& advected_velocity_x,
                      std::vector<float>& advected_velocity_y) const;

  void ComputeDivergence(std::vector<float> &divergence) const;

  void ComputePressure(const std::vector<float> &divergence, std::vector<float> &pressure) const;

  void ComputeCurlField(const std::vector<float> &pressure,
                        std::vector<float> &curl_x,
                        std::vector<float> &curl_y) const;

  void CorrectBoundaryDensities(std::vector<float>& densities) const;

  void CorrectBoundaryVelocities(std::vector<float>& velocity_x,
                                 std::vector<float>& velocity_y) const;

  static inline float Lerp(float from, float to, float pct) { return from + pct * (to - from); }

  float delta_t_;
  float diffusion_rate_;
};

#endif // GRID_FLUID_SIMULATOR_H
