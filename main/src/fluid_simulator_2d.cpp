#include "fluid_simulator_2d.h"

FluidSimulator2D::FluidSimulator2D(uint32_t dim_x, uint32_t dim_y) //
        : FluidSimulator()                                             //
        , dim_x_{dim_x}                                                //
        , dim_y_{dim_y}                                                //
        , num_cells_{dim_x_ * dim_y_}                                  //

{
  if (dim_x == 0 || dim_y == 0) {
    throw std::runtime_error("Width and heightmust be non-zero");
  }

  AllocateStorage();
}

void FluidSimulator2D::AllocateStorage() {
  density_.resize(num_cells_, 0);
  velocity_x_.resize(num_cells_, 0);
  velocity_y_.resize(num_cells_, 0);
}

// Override to set up an initial density
void FluidSimulator2D::Initialise() {
  InitialiseDensity();
  InitialiseVelocity();
}

const std::vector<float> &FluidSimulator2D::Density() const {
  return density_;
}

const std::vector<float> &FluidSimulator2D::VelocityX() const {
  return velocity_x_;
}

const std::vector<float> &FluidSimulator2D::VelocityY() const {
  return velocity_y_;
}

void FluidSimulator2D::AddDensity(uint32_t x, uint32_t y, float amount) {
  density_.at(Index(x, y)) += amount;
}
