#include "grid_fluid_rgb_simulator.h"
#include <QThread>
#include "spdlog/spdlog.h"
#include <cmath>
#include <iostream>

const float FLOW_RATE = 0.1f;

GridFluidRGBSimulator::GridFluidRGBSimulator(uint32_t width,      //
                                             uint32_t height,     //
                                             float delta_t,       //
                                             float diffusion_rate //
)                    //
        : GridFluidSimulator{width, height, delta_t, diffusion_rate}  //
        , density_g_(num_cells_, 0)                                   //
        , density_b_(num_cells_, 0)                                   //
{
  InitialiseDensity();
  InitialiseVelocity();
}

void GridFluidRGBSimulator::InitialiseDensity() {
  std::fill(density_.begin(), density_.end(), 0.0f);
  std::fill(density_g_.begin(), density_g_.end(), 0.0f);
  std::fill(density_b_.begin(), density_b_.end(), 0.0f);
//  // Initialise with three blobs
//  auto rad = dim_x_ / 6.0f;
//  auto rad2 = rad * rad;
//
//  float cx_1 = dim_x_ / 2.0f;
//  float cx_2 = dim_x_ / 4.0f;
//  float cx_3 = 3 * dim_x_ / 4.0f;
//  float cy_1 = 3 * dim_y_ / 4.0f;
//  float cy_2 = dim_y_ / 4.0f;
//  for (int y = 0; y < dim_y_; y++) {
//    for (int x = 0; x < dim_x_; x++) {
//      auto dx = x - cx_1;
//      auto dy = y - cy_1;
//      density_.at(Index(x, y)) = (dx * dx + dy * dy < rad2) ? 1.0f : 0.0f;
//      dx = x - cx_2;
//      dy = y - cy_2;
//      density_g_.at(Index(x, y)) = (dx * dx + dy * dy < rad2) ? 1.0f : 0.0f;
//      dx = x - cx_3;
//      dy = y - cy_2;
//      density_b_.at(Index(x, y)) = (dx * dx + dy * dy < rad2) ? 1.0f : 0.0f;
//    }
//  }
}

void GridFluidRGBSimulator::InitialiseVelocity() {
  std::fill(velocity_x_.begin(), velocity_x_.end(), 0.0f);
  std::fill(velocity_y_.begin(), velocity_y_.end(), 0.0f);

//    // Initialise a spiral
//    /*
//     * fx   |   fy   |   vx  |  vy
//     * -----+--------+-------+-----
//     *   0  |   .01  |  0.0  | 0.5
//     *   0  |  -.01  |  0.0  |-0.5
//     *   0  |   .5   |  0.0  | 0.0
//     *   0  |  -.5   |  0.0  |-0.0
//     */
//    float cx = dim_x_ * 0.5f;
//    float cy = dim_y_ * 0.5f;
//    for (int y = 0; y < dim_y_; y++) {
//      for (int x = 0; x < dim_x_; x++) {
//	auto fx = 2.f * (x - cx) / dim_x_;
//	auto fy = 2.f * (y - cy) / dim_y_;
//	velocity_x_.at(Index(x, y)) = fx - fy - fx * (fx * fx + fy * fy);
//	velocity_y_.at(Index(x, y)) = fx + fy - fy * (fx * fx + fy * fy);
//      }
//    }
}

void GridFluidRGBSimulator::Simulate() {

  ProcessSources();
  CorrectBoundaryVelocities(velocity_x_, velocity_y_);
  CorrectBoundaryDensities(density_);
  CorrectBoundaryDensities(density_g_);
  CorrectBoundaryDensities(density_b_);

  std::vector<float> temp_density(num_cells_, 0);
  std::vector<float> temp_velocity_x(num_cells_, 0);
  std::vector<float> temp_velocity_y(num_cells_, 0);

  Diffuse(density_, temp_density);
  std::memcpy(density_.data(), temp_density.data(), num_cells_ * sizeof(float));
  AdvectDensity(density_, temp_density);
  std::memcpy(density_.data(), temp_density.data(), num_cells_ * sizeof(float));

  Diffuse(density_g_, temp_density);
  std::memcpy(density_g_.data(), temp_density.data(), num_cells_ * sizeof(float));
  AdvectDensity(density_g_, temp_density);
  std::memcpy(density_g_.data(), temp_density.data(), num_cells_ * sizeof(float));

  Diffuse(density_b_, temp_density);
  std::memcpy(density_b_.data(), temp_density.data(), num_cells_ * sizeof(float));
  AdvectDensity(density_b_, temp_density);
  std::memcpy(density_b_.data(), temp_density.data(), num_cells_ * sizeof(float));

  Diffuse(velocity_x_, temp_velocity_x);
  Diffuse(velocity_y_, temp_velocity_y);
  CorrectBoundaryVelocities(temp_velocity_x, temp_velocity_y);
  std::memcpy(velocity_x_.data(), temp_velocity_x.data(), num_cells_ * sizeof(float));
  std::memcpy(velocity_y_.data(), temp_velocity_y.data(), num_cells_ * sizeof(float));
  SuppressDivergence();

  AdvectVelocity(temp_velocity_x, temp_velocity_y);
  std::memcpy(velocity_x_.data(), temp_velocity_x.data(), num_cells_ * sizeof(float));
  std::memcpy(velocity_y_.data(), temp_velocity_y.data(), num_cells_ * sizeof(float));
  SuppressDivergence();
}

const std::vector<float> &GridFluidRGBSimulator::DensityGreen() const {
  return density_g_;
}

const std::vector<float> &GridFluidRGBSimulator::DensityBlue() const {
  return density_b_;
}
