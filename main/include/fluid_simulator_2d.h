#ifndef FLUID_SIMULATOR_2D_H
#define FLUID_SIMULATOR_2D_H

#include "fluid_simulator.h"

class FluidSimulator2D : public FluidSimulator {
public:
  FluidSimulator2D(uint32_t dim_x, uint32_t dim_y);

  virtual void Simulate() = 0;

  const uint32_t DimX() const { return dim_x_; }

  const uint32_t DimY() const { return dim_y_; }

  virtual const std::vector<float> &Density() const;

  virtual const std::vector<float> &VelocityX() const;

  virtual const std::vector<float> &VelocityY() const;

  virtual void AddDensity(uint32_t x, uint32_t y, float amount);

  virtual void Initialise();

protected:
  inline uint32_t Index(uint32_t x, uint32_t y) const { return y * dim_x_ + x; };

  virtual void InitialiseDensity() {};

  virtual void InitialiseVelocity() {};
  uint32_t dim_x_;
  uint32_t dim_y_;
  uint32_t num_cells_;
  std::vector<float> density_;
  std::vector<float> velocity_x_;
  std::vector<float> velocity_y_;

private:
  void AllocateStorage();
};

#endif // FLUID_SIMULATOR_2D_H
