#include "fluid_simulator_2d.h"

[[maybe_unused]] FluidSimulator2D::FluidSimulator2D(uint32_t dim_x, uint32_t dim_y) //
        : FluidSimulator()                                             //
        , dim_x_{dim_x}                                                //
        , dim_y_{dim_y}                                                //
        , num_cells_{dim_x_ * dim_y_}                                  //

{
  if (dim_x == 0 || dim_y == 0) {
    throw std::runtime_error("Width and height must be non-zero");
  }

  AllocateStorage();
}

void FluidSimulator2D::AllocateStorage() {
  density_.resize(num_cells_, 0);
  velocity_x_.resize(num_cells_, 0);
  velocity_y_.resize(num_cells_, 0);
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

void FluidSimulator2D::AddSource(uint32_t x, uint32_t y, float amount, float velocity_x, float velocity_y){
  auto idx = Index(x,y);
  source_[idx] = {amount, velocity_x, velocity_y};
}

[[maybe_unused]] void FluidSimulator2D::AddSource(uint32_t x, uint32_t y, float amount, float velocity_x, float velocity_y) {
  sources_[Index(x, y)] = {amount, velocity_x, velocity_y};
}

[[maybe_unused]] void FluidSimulator2D::ClearSources() {
  sources_.clear();
}

[[maybe_unused]] void FluidSimulator2D::RemoveSource(uint32_t x, uint32_t y){
  auto idx = Index(x, y);
  if( sources_.count(idx)) sources_.erase(idx);
}

[[maybe_unused]] void FluidSimulator2D::ProcessSources(){
  for( const auto & source : sources_){
    auto idx = source.first;
    auto amount = std::get<0>(source.second);
    auto vx = std::get<1>(source.second);
    auto vy = std::get<2>(source.second);
    density_.at(idx) = amount;
    velocity_x_.at(idx) = vx;
    velocity_y_.at(idx) = vy;
  }
}
