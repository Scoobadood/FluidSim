#ifndef FLUID_SIMULATOR_2D_H
#define FLUID_SIMULATOR_2D_H

#include <map>
#include "fluid_simulator.h"

class FluidSimulator2D : public FluidSimulator {
public:
  [[maybe_unused]] FluidSimulator2D(uint32_t dim_x, uint32_t dim_y);

  void Simulate() override = 0;

  [[nodiscard]] uint32_t DimX() const { return dim_x_; }

  [[nodiscard]] uint32_t DimY() const { return dim_y_; }

  [[nodiscard]] const std::vector<float> &Density() const override;

  [[nodiscard]] virtual const std::vector<float> &VelocityX() const;

  [[nodiscard]] virtual const std::vector<float> &VelocityY() const;

  virtual void AddDensity(uint32_t x, uint32_t y, float amount);

  [[maybe_unused]] void AddSource(uint32_t x, uint32_t y, float amount, float velocity_x, float velocity_y);

  [[maybe_unused]] void ClearSources();

  [[maybe_unused]] void RemoveSource(uint32_t x, uint32_t y);

protected:
  [[maybe_unused]] void ProcessSources();

  [[nodiscard]] inline uint32_t Index(uint32_t x, uint32_t y) const { return y * dim_x_ + x; };

  uint32_t dim_x_;
  uint32_t dim_y_;
  uint32_t num_cells_;
  std::vector<float> density_;
  std::vector<float> velocity_x_;
  std::vector<float> velocity_y_;

  std::map<uint32_t, std::tuple<float, float, float>> source_;

private:
  void AllocateStorage();
};

#endif // FLUID_SIMULATOR_2D_H
