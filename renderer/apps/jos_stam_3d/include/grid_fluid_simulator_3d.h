#ifndef GRID_FLUID_SIMULATOR_H
#define GRID_FLUID_SIMULATOR_H

#include <cstdint>
#include <vector>
#include <map>

class GridFluidSimulator3D {
 public:
  GridFluidSimulator3D(uint32_t size,      //
                       float diffusion_rate //
  );

  void Simulate(float delta_t);

  void Initialise();

 protected:
  void Diffuse(const std::vector<float> &current_density,
               float delta_t,
               std::vector<float> &next_density);

  void SuppressDivergence();

 private:
  void AllocateStorage();
  [[maybe_unused]] void ProcessSources();

  [[nodiscard]]
  float AdvectValue(const std::vector<float> &velocity_x,
                    const std::vector<float> &velocity_y,
                    const std::vector<float> &source_data,
                    uint32_t x, uint32_t y,
                    float delta_t) const;

  [[nodiscard]] inline uint32_t Index(uint32_t x, uint32_t y) const { return y * dim_x_ + x; };

  void AdvectDensity(const std::vector<float> &curr_density,
                     float delta_t,
                     std::vector<float> &next_density) const;

  void AdvectVelocity(std::vector<float> &advected_velocity_x,
                      std::vector<float> &advected_velocity_y,
                      float delta_t) const;

  void ComputeDivergence(std::vector<float> &divergence) const;

  void ComputePressure(const std::vector<float> &divergence, std::vector<float> &pressure) const;

  void ComputeCurlField(const std::vector<float> &pressure,
                        std::vector<float> &curl_x,
                        std::vector<float> &curl_y) const;

  void CorrectBoundaryDensities(std::vector<float> &densities) const;

  void CorrectBoundaryVelocities(std::vector<float> &velocity_x,
                                 std::vector<float> &velocity_y) const;

  static inline float Lerp(float from, float to, float pct) { return from + pct * (to - from); }

  uint32_t dim_x_;
  uint32_t dim_y_;
  uint32_t dim_z_;
  uint32_t num_cells_;
  std::vector<float> density_;
  std::vector<float> velocity_x_;
  std::vector<float> velocity_y_;
  std::vector<float> velocity_z_;

  float diffusion_rate_;

  std::map<uint32_t, std::tuple<float, float, float>> sources_;
};

#endif // GRID_FLUID_SIMULATOR_H
