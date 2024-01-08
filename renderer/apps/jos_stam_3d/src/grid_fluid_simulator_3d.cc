#include "grid_fluid_simulator_3d.h"
#include "spdlog/spdlog.h"

#include <cmath>

const uint32_t NUM_GS_ITERS = 10;

GridFluidSimulator3D::GridFluidSimulator3D(uint32_t size,       //
                                           float diffusion_rate //
)                                                               //
    : dim_x_{size}                                              //
    , dim_y_{size}                                              //
    , dim_z_{size}                                              //
    , num_cells_{size * size}                            //
    , diffusion_rate_{diffusion_rate}                           //
{
  if (size == 0) {
    throw std::runtime_error("Size must be non-zero");
  }

  AllocateStorage();
}

void GridFluidSimulator3D::Initialise() {
  // Set up an initial density and velocity field
}

void GridFluidSimulator3D::AddSource(uint32_t x, uint32_t y, float amount, float velocity_x, float velocity_y) {
  sources_[y * dim_x_ + x] = {amount, velocity_x, velocity_y};
}

void GridFluidSimulator3D::AllocateStorage() {
  density_.resize(num_cells_, 0);
  velocity_x_.resize(num_cells_, 0);
  velocity_y_.resize(num_cells_, 0);
  velocity_z_.resize(num_cells_, 0);
}

void GridFluidSimulator3D::Diffuse(const std::vector<float> &current_density,
                                   float delta_t,
                                   std::vector<float> &next_density) {
  // Initialise target_density with current values because why not
  std::memcpy(next_density.data(), current_density.data(), num_cells_ * sizeof(float));

  // Run four iterations of GS
  // Dn(x,y) = Dc(x,y) + (k*0.25*(Dn(x+1,y)+Dn(x-1,y)+Dn(x,y+1)+Dn(x,y-1)))/(1+k)

  auto k = delta_t * diffusion_rate_;
  for (auto iter = 0; iter < NUM_GS_ITERS; ++iter) {
    for (auto y = 1; y < dim_y_ - 1; ++y) {
      for (auto x = 1; x < dim_x_ - 1; ++x) {
        auto idx = Index(x, y);

        float total_nbrs = 0.0f;
        float nbr_count = 0.0f;
        if (x > 1) {
          total_nbrs += next_density.at(idx - 1);
          nbr_count += 1.0f;
        }
        if (x < dim_x_ - 1) {
          total_nbrs += next_density.at(idx + 1);
          nbr_count += 1.0f;
        }
        if (y > 0) {
          total_nbrs += next_density.at(idx - dim_x_);
          nbr_count += 1.0f;
        }
        if (y < dim_y_ - 1) {
          total_nbrs += next_density.at(idx + dim_x_);
          nbr_count += 1.0f;
        }
        auto mean_nbr = total_nbrs / nbr_count;

        //x[IX(i,j)] = (x0[IX(i,j)] + a*(x[IX(i-1,j)]+x[IX(i+1,j)]+x[IX(i,j-1)]+x[IX(i,j+1)]))/(1+4*a)

        auto new_val = (current_density.at(idx) + (k * mean_nbr)) / (k + 1.0f);
        next_density.at(idx) = new_val;
      }
    }
    CorrectBoundaryDensities(next_density);
  }
}

float GridFluidSimulator3D::AdvectValue(const std::vector<float> &velocity_x,
                                        const std::vector<float> &velocity_y,
                                        const std::vector<float> &source_data,
                                        uint32_t x, uint32_t y,
                                        float delta_t) const {
  auto idx = Index(x, y);

  // Get the velocity for this cell
  auto vx = velocity_x.at(idx);
  auto vy = velocity_y.at(idx);

  // Get the source point for that flow
  auto source_x = ((float) x + 0.5f) - vx * delta_t;
  auto source_y = ((float) y + 0.5f) - vy * delta_t;
  source_x = std::fmaxf(0.5f, std::fminf((float) dim_x_ - 0.5f, source_x));
  source_y = std::fmaxf(0.5f, std::fminf((float) dim_y_ - 0.5f, source_y));

  /* 0     1     2     3
   * +-----+-----+-----+  0
   * |     |     |     |
   * |     |     |     |
   * |     |     |     |
   * +-----+-----+-----+  1
   * |     |     |     |
   * |  o  |  o  |     |
   * |     |x    |     |
   * +-----+-----+-----+  2
   * |     |     |     |
   * |  o  |  o  |     |
   * |     |     |     |
   * +-----+-----+-----+  3
   */
  // Get the base coord
  auto base_x = std::floorf(source_x - 0.5f);
  auto base_y = std::floorf(source_y - 0.5f);

  // And the fractional offset
  auto frac_x = source_x - base_x - 0.5f;
  auto frac_y = source_y - base_y - 0.5f;

  // Interpolate top and bottom
  auto bl = source_data.at(Index((uint32_t) base_x, (uint32_t) base_y));
  auto br = source_data.at(Index((uint32_t) base_x + 1, (uint32_t) base_y));
  auto tl = source_data.at(Index((uint32_t) base_x, (uint32_t) base_y + 1));
  auto tr = source_data.at(Index((uint32_t) base_x + 1, (uint32_t) base_y + 1));
  auto top_lerp = Lerp(tl, tr, frac_x);
  auto btm_lerp = Lerp(bl, br, frac_x);
  auto final_val = Lerp(btm_lerp, top_lerp, frac_y);
  // interpolate top to bottom
  return final_val;
}

void GridFluidSimulator3D::AdvectDensity(const std::vector<float> &curr_density,
                                         float delta_t,
                                         std::vector<float> &advected_density) const {
  std::fill(advected_density.begin(), advected_density.end(), 0.0f);
  for (auto y = 1; y < dim_y_ - 1; ++y) {
    for (auto x = 1; x < dim_x_ - 1; ++x) {
      advected_density.at(Index(x, y)) = AdvectValue(velocity_x_, velocity_y_, curr_density, x, y, delta_t);
    }
  }
  CorrectBoundaryDensities(advected_density);
}

void GridFluidSimulator3D::AdvectVelocity(std::vector<float> &advected_velocity_x,
                                          std::vector<float> &advected_velocity_y,
                                          float delta_t) const {
  for (auto y = 1; y < dim_y_ - 1; ++y) {
    for (auto x = 1; x < dim_x_ - 1; ++x) {
      auto idx = Index(x, y);
      advected_velocity_x.at(idx) = AdvectValue(velocity_x_, velocity_y_, velocity_x_, x, y, delta_t);
      advected_velocity_y.at(idx) = AdvectValue(velocity_x_, velocity_y_, velocity_y_, x, y, delta_t);
    }
  }
  CorrectBoundaryVelocities(advected_velocity_x, advected_velocity_y);
}

/*
 * d(x,y) = [ vx(x+1,y) - vx(x-1,y) + vy(x,y+1)-vy(x,y-1) ] * 0.5f
 */
void GridFluidSimulator3D::ComputeDivergence(std::vector<float> &divergence) const {
  for (auto y = 1; y < dim_y_ - 1; ++y) {
    for (auto x = 1; x < dim_x_ - 1; ++x) {
      auto idx = Index(x, y);
      divergence.at(idx) =                   //
          (                                  //
              velocity_x_.at(idx + 1) -      //
                  velocity_x_.at(idx - 1) +      //
                  velocity_y_.at(idx + dim_x_) - //
                  velocity_y_.at(idx - dim_x_)   //
          )
              * 0.5f;
    }
  }
}

/*
 * Compute pressure and solve to obtain stable field
 * 0.25f * [p(x-1,y)+p(x+1,y)+p(x,y-1)+p(x,y+1)- divergence(x,y)] =p(x,y)
 */
void GridFluidSimulator3D::ComputePressure(const std::vector<float> &divergence,
                                           std::vector<float> &pressure) const {
  // Set pressure to zero everywhere
  std::fill(pressure.begin(), pressure.end(), 0);
  std::vector<float> temp_pressure(num_cells_, 0);
  std::fill(temp_pressure.begin(), temp_pressure.end(), 0);

  // TODO: Parameterise this count
  for (auto iter = 0; iter < NUM_GS_ITERS; ++iter) {
    for (auto y = 1; y < dim_y_ - 1; ++y) {
      for (auto x = 1; x < dim_x_ - 1; ++x) {
        auto idx = Index(x, y);

        auto p = (                               //
            pressure.at(idx - 1) +      //
                pressure.at(idx + 1) +      //
                pressure.at(idx - dim_x_) + //
                pressure.at(idx + dim_x_) - //
                divergence.at(idx)          //
        )
            * 0.25f;
        temp_pressure.at(idx) = p;
      }
    }

    std::memcpy(pressure.data(), temp_pressure.data(), num_cells_ * sizeof(float));
  }
}

/*
 * \nabla p(x,y) =0.5f * [  p(x+1,y) - p(x-1),y), p(x,y+1)-p(x,y-1) ]
 */
void GridFluidSimulator3D::ComputeCurlField(const std::vector<float> &pressure,
                                            std::vector<float> &curl_x,
                                            std::vector<float> &curl_y) const {
  for (auto y = 1; y < dim_y_ - 1; ++y) {
    for (auto x = 1; x < dim_x_ - 1; ++x) {
      auto idx = Index(x, y);
      curl_x.at(idx) = (pressure.at(idx + 1) - pressure.at(idx - 1)) * 0.5f;
      curl_y.at(idx) = (pressure.at(idx + dim_x_) - pressure.at(idx - dim_x_)) * 0.5f;
    }
  }
}

void GridFluidSimulator3D::SuppressDivergence() {
  std::vector<float> divergence(num_cells_, 0);
  ComputeDivergence(divergence);
  std::vector<float> pressure(num_cells_, 0);
  ComputePressure(divergence, pressure);
  std::vector<float> curl_x(num_cells_, 0);
  std::vector<float> curl_y(num_cells_, 0);
  ComputeCurlField(pressure, curl_x, curl_y);

  for (auto i = 0; i < num_cells_; ++i) {
    velocity_x_.at(i) -= curl_x.at(i);
    velocity_y_.at(i) -= curl_y.at(i);
  }
  CorrectBoundaryVelocities(velocity_x_, velocity_y_);
}

void GridFluidSimulator3D::CorrectBoundaryDensities(std::vector<float> &densities) const {
  // Horizontal boundaries
  for (auto x = 1; x < dim_x_ - 1; ++x) {
    // Top
    densities.at(Index(x, 0)) = densities.at(Index(x, 1));
    densities.at(Index(x, dim_y_ - 1)) = densities.at(Index(x, dim_y_ - 2));
  }
  // Vertical boundaries
  for (auto y = 1; y < dim_y_ - 1; ++y) {
    densities.at(Index(0, y)) = densities.at(Index(1, y));
    densities.at(Index(dim_x_ - 1, y)) = densities.at(Index(dim_x_ - 2, y));
  }
  densities.at(Index(0, 0)) = 0.5f * (densities.at(Index(1, 0)) + densities.at(Index(0, 1)));
  densities.at(Index(0, dim_y_ - 1)) = 0.5f * (densities.at(Index(0, dim_y_ - 2)) + densities.at(Index(1, dim_y_ - 1)));
  densities.at(Index(dim_x_ - 1, 0)) = 0.5f * (densities.at(Index(dim_x_ - 2, 0)) + densities.at(Index(dim_x_ - 1, 1)));
  densities.at(Index(dim_x_ - 1, dim_y_ - 1)) =
      0.5f * (densities.at(Index(dim_x_ - 2, dim_y_ - 1)) + densities.at(Index(dim_x_ - 1, dim_y_ - 2)));
}

/**
 * For all boundaries, set the appropriate Velocities
 * @param velocity_x
 * @param velocity_y
 * @param velocity_z
 */
void GridFluidSimulator3D::CorrectBoundaryVelocities(std::vector<float> &velocity_x,
                                                     std::vector<float> &velocity_y) const {
  // Horizontal boundaries
  for (auto x = 1; x < dim_x_ - 1; ++x) {
    // Top
    velocity_x.at(Index(x, 0)) = velocity_x.at(Index(x, 1));
    velocity_y.at(Index(x, 0)) = 0; //-velocity_y.at(Index(x, 1));

    // Bottom
    velocity_x.at(Index(x, dim_y_ - 1)) = velocity_x.at(Index(x, dim_y_ - 2));
    velocity_y.at(Index(x, dim_y_ - 1)) = 0; //-velocity_y.at(Index(x, dim_y_ - 2));
  }
  // Vertical boundaries
  for (auto y = 1; y < dim_y_ - 1; ++y) {
    // Left
    velocity_x.at(Index(0, y)) = 0; //-velocity_x.at(Index(1, y));
    velocity_y.at(Index(0, y)) = velocity_y.at(Index(1, y));
    // Right
    velocity_x.at(Index(dim_x_ - 1, y)) = 0; //-velocity_x.at(Index(dim_x_ - 2, y));
    velocity_y.at(Index(dim_x_ - 1, y)) = velocity_y.at(Index(dim_x_ - 2, y));
  }
  velocity_x.at(Index(0, 0)) = 0.5f * (velocity_x.at(Index(1, 0)) + velocity_x.at(Index(0, 1)));
  velocity_x.at(Index(0, dim_y_ - 1)) =
      0.5f * (velocity_x.at(Index(0, dim_y_ - 2)) + velocity_x.at(Index(1, dim_y_ - 1)));
  velocity_x.at(Index(dim_x_ - 1, 0)) =
      0.5f * (velocity_x.at(Index(dim_x_ - 2, 0)) + velocity_x.at(Index(dim_x_ - 1, 1)));
  velocity_x.at(Index(dim_x_ - 1, dim_y_ - 1)) =
      0.5f * (velocity_x.at(Index(dim_x_ - 2, dim_y_ - 1)) + velocity_x.at(Index(dim_x_ - 1, dim_y_ - 2)));

  velocity_y.at(Index(0, 0)) = 0.5f * (velocity_y.at(Index(1, 0)) + velocity_y.at(Index(0, 1)));
  velocity_y.at(Index(0, dim_y_ - 1)) =
      0.5f * (velocity_y.at(Index(0, dim_y_ - 2)) + velocity_y.at(Index(1, dim_y_ - 1)));
  velocity_y.at(Index(dim_x_ - 1, 0)) =
      0.5f * (velocity_y.at(Index(dim_x_ - 2, 0)) + velocity_y.at(Index(dim_x_ - 1, 1)));
  velocity_y.at(Index(dim_x_ - 1, dim_y_ - 1)) =
      0.5f * (velocity_y.at(Index(dim_x_ - 2, dim_y_ - 1)) + velocity_y.at(Index(dim_x_ - 1, dim_y_ - 2)));
}

/**
 * For any sources which are adding density to the grid, add more.
 */
[[maybe_unused]] void GridFluidSimulator3D::ProcessSources() {
  for (const auto &source : sources_) {
    auto idx = source.first;
    auto amount = std::get<0>(source.second);
    auto vx = std::get<1>(source.second);
    auto vy = std::get<2>(source.second);
    density_.at(idx) = amount;
    velocity_x_.at(idx) = vx;
    velocity_y_.at(idx) = vy;
  }
}

/**
 * Run one step of simulation.
 * @param delta_t Elapsed time since last step.
 */
void GridFluidSimulator3D::Simulate(float delta_t) {
  std::vector<float> temp_density(num_cells_, 0);
  std::vector<float> temp_velocity_x(num_cells_, 0);
  std::vector<float> temp_velocity_y(num_cells_, 0);

  ProcessSources();
  CorrectBoundaryDensities(density_);
  CorrectBoundaryVelocities(velocity_x_, velocity_y_);

  Diffuse(density_, delta_t, temp_density);
  std::memcpy(density_.data(), temp_density.data(), num_cells_ * sizeof(float));

  AdvectDensity(density_, delta_t, temp_density);
  std::memcpy(density_.data(), temp_density.data(), num_cells_ * sizeof(float));

  Diffuse(velocity_x_, delta_t, temp_velocity_x);
  Diffuse(velocity_y_, delta_t, temp_velocity_y);
  CorrectBoundaryVelocities(temp_velocity_x, temp_velocity_y);
  std::memcpy(velocity_x_.data(), temp_velocity_x.data(), num_cells_ * sizeof(float));
  std::memcpy(velocity_y_.data(), temp_velocity_y.data(), num_cells_ * sizeof(float));

  AdvectVelocity(temp_velocity_x, temp_velocity_y, delta_t);
  std::memcpy(velocity_x_.data(), temp_velocity_x.data(), num_cells_ * sizeof(float));
  std::memcpy(velocity_y_.data(), temp_velocity_y.data(), num_cells_ * sizeof(float));

  SuppressDivergence();
}