#ifndef MIKEASHFLUIDSIMULATOR_H
#define MIKEASHFLUIDSIMULATOR_H

#include "fluid_simulator.h"

#include <vector>

class MikeAshFluidSimulator : public FluidSimulator {
public:
  MikeAshFluidSimulator(uint32_t width,    //
                        uint32_t height,   //
                        uint32_t depth,    //
                        float delta_t,     //
                        float diffuseness, //
                        float viscosity    //
  );

  const inline size_t Index(uint32_t x, uint32_t y, uint32_t z) const {
    assert(x >= 0 && x <= width_);
    assert(y >= 0 && y <= height_);
    assert(z >= 0 && z <= depth_);
    return (x + y * width_ + z * width_ * height_);
  }

  void AddDensity(uint32_t x, uint32_t y, uint32_t z, float amount);

  void AddVelocity(uint32_t x, //
                   uint32_t y,
                   uint32_t z,
                   float amount_x,
                   float amount_y,
                   float amount_z);

  const std::vector<float> &Density() const override;

  void Simulate() override;

protected:
  void SetBoundary(uint32_t boundary, std::vector<float> &velocities);

  void LinearSolve(uint32_t boundary,
                   std::vector<float> &new_values,
                   const std::vector<float> &old_values,
                   float a,
                   float c,
                   uint32_t num_iterations,
                   uint32_t dim_1,
                   uint32_t dim_2);

  void Diffuse(uint32_t boundary,
               std::vector<float> &next_velocity,
               const std::vector<float> &prev_velocity,
               float diffusion_value,
               float delta_t,
               uint32_t num_iterations,
               uint32_t dim_1,
               uint32_t dim_2);

  void Project(const std::vector<float> &prev_velocity_x,
               const std::vector<float> &prev_velocity_y,
               const std::vector<float> &prev_velocity_z,
               std::vector<float> &next_velocity_x,
               std::vector<float> &next_velocity_y,
               uint32_t num_neighbours);

  void Advect(uint32_t axis,
              std::vector<float> &next_velocity,
              const std::vector<float> &prev_velocity,
              const std::vector<float> &prev_velocity_x,
              const std::vector<float> &prev_velocity_y,
              const std::vector<float> &prev_velocity_z,
              uint32_t num_neighbours);

private:
  uint32_t width_;
  uint32_t height_;
  uint32_t depth_;

  // Simulator timestep
  float delta_t_;

  float diffuseness_;

  float viscosity_;

  std::vector<float> s_;

  std::vector<float> density_;

  std::vector<float> velocity_x_;
  std::vector<float> velocity_y_;
  std::vector<float> velocity_z_;

  std::vector<float> velocity_x_prev_;
  std::vector<float> velocity_y_prev_;
  std::vector<float> velocity_z_prev_;
};

#endif // MIKEASHFLUIDSIMULATOR_H
