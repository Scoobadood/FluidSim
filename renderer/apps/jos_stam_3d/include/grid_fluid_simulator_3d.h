#ifndef GRID_FLUID_SIMULATOR_H
#define GRID_FLUID_SIMULATOR_H

#include <cstdint>
#include <vector>
#include <map>
#include "vectors.h"

class GridFluidSimulator3D {
 public:
  GridFluidSimulator3D(uint32_t num_cells_x,
                       uint32_t num_cells_y,
                       uint32_t num_cells_z,
                       float size_x,
                       float size_y,
                       float size_z,
                       float diffusion_rate,
                       float dissipation_rate,
                       float viscosity);

  void Simulate(const std::vector<float> &sources,
                const std::vector<vec3f> &forces, float delta_);

  const std::vector<float> &Density() const {
    return content_;
  }

 private:
  void AllocateStorage();

  // Velocity Solver
  void VelocityStep(const std::vector<vec3f> &forces, float delta_);
  void ApplyForces(const std::vector<vec3f> &forces, float delta_);
  void AdvectVelocity(float delta_t);
  void DiffuseVelocity(float delta_t);
  void Project();

  // Scalar solver
  void ScalarStep(const std::vector<float> &sources, float delta_);
  void ApplySources(const std::vector<float> &sources, float delta_);
  void AdvectScalar(float delta_t);
  void DiffuseScalar(float delta_t);
  void Dissipate(float delta_t);

  void Diffuse(std::vector<float> &current_amount,
               float diffusion_rate_,
               float delta_t,
               bool is_velocity);

  float AdvectValue(const std::vector<float> &velocity_x,
                    const std::vector<float> &velocity_y,
                    const std::vector<float> &velocity_z,
                    const std::vector<float> &source_data,
                    uint32_t x, uint32_t y, uint32_t z,
                    float delta_t) const;

  // Compute an index into data arrays given x,y,z coords. Does not check bounds.
  inline uint32_t Index(uint32_t x, uint32_t y, uint32_t z) const {
    return (z * num_cells_.x * num_cells_.y) + (y * num_cells_.x) + x;
  };

  void SetBoundary(std::vector<float> &src, float scale);

  static inline float Lerp(float from, float to, float pct) { return from + pct * (to - from); }

  vec3u num_cells_;
  vec3f size_;
  vec3f cell_size_;
  uint32_t total_cells_;
  std::vector<float> content_;
  std::vector<float> velocity_x_;
  std::vector<float> velocity_y_;
  std::vector<float> velocity_z_;

  float diffusion_rate_;
  float dissipation_rate_;
  float viscosity_;
  float density_;
};

#endif // GRID_FLUID_SIMULATOR_H
