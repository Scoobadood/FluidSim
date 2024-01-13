#include "grid_fluid_simulator_3d.h"
#include "spdlog/spdlog.h"

#include <cmath>

const uint32_t NUM_GS_ITERS = 20;

GridFluidSimulator3D::GridFluidSimulator3D(uint32_t num_cells_x,    //
                                           uint32_t num_cells_y,    //
                                           uint32_t num_cells_z,    //
                                           float size_x,          //
                                           float size_y,          //
                                           float size_z,          //
                                           float diffusion_rate,     //
                                           float dissipation_rate,   //
                                           float viscosity           //
)                                                                    //
    : num_cells_{num_cells_x, num_cells_y, num_cells_z}     //
    , size_{size_x, size_y, size_z}                         //
    , cell_size_{0.0f, 0.0f, 0.0f}                          //
    , total_cells_{num_cells_x * num_cells_y * num_cells_z}          //
    , diffusion_rate_{diffusion_rate}                                //
    , dissipation_rate_{dissipation_rate}                            //
    , viscosity_{viscosity}                                          //
    , density_{1.0f}                                                 //
{
  if (num_cells_x == 0 || num_cells_y == 0 || num_cells_z == 0) {
    throw std::runtime_error("Size must be non-zero");
  }

  cell_size_ = {size_x / (float) num_cells_x,
                size_y / (float) num_cells_y,
                size_z / (float) num_cells_z};

  AllocateStorage();
}

void GridFluidSimulator3D::AllocateStorage() {
  content_.resize(total_cells_, 0);
  velocity_x_.resize(total_cells_, 0);
  velocity_y_.resize(total_cells_, 0);
  velocity_z_.resize(total_cells_, 0);
}

/* ************************************************************************
 * **
 * ** Utilities
 * **
 * ************************************************************************/
/**
 * Backward advection solver for a value
 */
float GridFluidSimulator3D::AdvectValue(const std::vector<float> &velocity_x,
                                        const std::vector<float> &velocity_y,
                                        const std::vector<float> &velocity_z,
                                        const std::vector<float> &source_data,
                                        uint32_t x, uint32_t y, uint32_t z,
                                        float delta_t) const {
  auto idx = Index(x, y, z);

  // Compute the dest world coord; the midpoint of this cell
  vec3f dest = {((float) x + 0.5f) * cell_size_.x,
                ((float) y + 0.5f) * cell_size_.y,
                ((float) z + 0.5f) * cell_size_.z};

  // Get the velocity for this cell
  vec3f velocity = {velocity_x.at(idx), velocity_y.at(idx), velocity_z.at(idx)};

  // Get the source point for that flow
  vec3f source = dest - (velocity * delta_t);

  // Force into bounds
  source.x = std::fmaxf(0.5f, std::fminf(size_.x - 1.5f, source.x));
  source.y = std::fmaxf(0.5f, std::fminf(size_.y - 1.5f, source.y));
  source.z = std::fmaxf(0.5f, std::fminf(size_.z - 1.5f, source.z));

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
  // Get the base cell. We then lerp between basec_cell and +1 in each direction
  vec3u base_cell = vec3u{
      (uint32_t) std::floorf((source.x / cell_size_.x) - 0.5f),
      (uint32_t) std::floorf((source.y / cell_size_.y) - 0.5f),
      (uint32_t) std::floorf((source.z / cell_size_.z) - 0.5f)
  };

  // Frac dist is source.x - midpoint-base-x
  // = source.x - ((basex.x +0.5) * cell_width)
  vec3f frac = vec3f{
      source.x - (((float) base_cell.x + 0.5f) * cell_size_.x),
      source.y - (((float) base_cell.y + 0.5f) * cell_size_.y),
      source.z - (((float) base_cell.z + 0.5f) * cell_size_.z)
  };

// Tri-linear interpolation over...
  auto blf = source_data.at(Index(base_cell.x, base_cell.y, base_cell.z));
  auto blb = source_data.at(Index(base_cell.x, base_cell.y, base_cell.z + 1));
  auto brf = source_data.at(Index(base_cell.x + 1, base_cell.y, base_cell.z));
  auto brb = source_data.at(Index(base_cell.x + 1, base_cell.y, base_cell.z + 1));
  auto tlf = source_data.at(Index(base_cell.x, base_cell.y + 1, base_cell.z));
  auto tlb = source_data.at(Index(base_cell.x, base_cell.y + 1, base_cell.z + 1));
  auto trf = source_data.at(Index(base_cell.x + 1, base_cell.y + 1, base_cell.z));
  auto trb = source_data.at(Index(base_cell.x + 1, base_cell.y + 1, base_cell.z + 1));

  auto top_back_lerp = Lerp(tlb, trb, frac.x);
  auto top_front_lerp = Lerp(tlf, trf, frac.x);
  auto top_lerp = Lerp(top_front_lerp, top_back_lerp, frac.z);

  auto btm_back_lerp = Lerp(blb, brb, frac.x);
  auto btm_front_lerp = Lerp(blf, brf, frac.x);
  auto btm_lerp = Lerp(btm_front_lerp, btm_back_lerp, frac.z);

  auto final_val = Lerp(btm_lerp, top_lerp, frac.y);
  return final_val;
}

void GridFluidSimulator3D::Diffuse(std::vector<float> &current_amount,
                                   float diffusion_rate, float delta_t,
                                   bool is_velocity) {

  // Initialise next_amount with current values (though any value would do)
  std::vector<float> diffused_content(total_cells_, 0);
  std::memcpy(diffused_content.data(), current_amount.data(), total_cells_ * sizeof(float));

  auto k = delta_t * diffusion_rate / (cell_size_.x * cell_size_.y * cell_size_.z);
  auto idx_dy = num_cells_.x;
  auto idx_dz = num_cells_.y * num_cells_.x;
  for (auto iter = 0; iter < NUM_GS_ITERS; ++iter) {

    for (auto z = 1; z < num_cells_.z - 1; ++z) {
      for (auto y = 1; y < num_cells_.y - 1; ++y) {
        for (auto x = 1; x < num_cells_.x - 1; ++x) {
          auto idx = Index(x, y, z);

          auto new_val = (
              current_amount.at(idx) + k *
                  (
                      diffused_content.at(idx - 1) + diffused_content.at(idx + 1) +
                          diffused_content.at(idx - idx_dy) + diffused_content.at(idx + idx_dy) +
                          diffused_content.at(idx - idx_dz) + diffused_content.at(idx + idx_dz)
                  )
          ) / (1 + 6.0f * k);
          diffused_content.at(idx) = new_val;
        }
      }
    }
    SetBoundary(diffused_content, is_velocity ? -1 : 1);
  }
  std::memcpy(current_amount.data(), diffused_content.data(), sizeof(float) * total_cells_);
}

void GridFluidSimulator3D::SetBoundary(std::vector<float> &src, float scale) {
  auto mx = num_cells_.x - 1;
  auto my = num_cells_.y - 1;
  auto mz = num_cells_.z - 1;

  for (auto z = 1; z < mz; ++z) {
    for (auto y = 1; y < my; ++y) {
      src[Index(0, y, z)] = scale * src[Index(1, y, z)];
      src[Index(mx, y, z)] = scale * src[Index(mx - 1, y, z)];
    }
  }
  for (auto z = 1; z < mz; ++z) {
    for (auto x = 1; x < mx; ++x) {
      src[Index(x, 0, z)] = scale * src[Index(x, 1, z)];
      src[Index(x, my, z)] = scale * src[Index(x, my - 1, z)];
    }
  }
  for (auto y = 1; y < my; ++y) {
    for (auto x = 1; x < mx; ++x) {
      src[Index(x, y, 0)] = scale * src[Index(x, y, 1)];
      src[Index(x, y, mz)] = scale * src[Index(x, y, mz - 1)];
    }
  }

  src[Index(0, 0, 0)] = 0.333f * (
      src[Index(1, 0, 0)] +
          src[Index(0, 1, 0)] +
          src[Index(0, 0, 1)]);

  src[Index(0, my, 0)] = 0.333f * (
      src[Index(0, my - 1, 0)]
          + src[Index(1, my, 0)]
          + src[Index(0, my, 1)]);

  src[Index(mx, my, 0)] = 0.333f * (
      src[Index(mx - 1, my, 0)]
          + src[Index(mx, my - 1, 0)]
          + src[Index(mx, my, 1)]);

  src[Index(mx, 0, 0)] = 0.333f * (
      src[Index(mx - 1, 0, 0)]
          + src[Index(mx, 1, 0)]
          + src[Index(mx, 0, 1)]);

  src[Index(0, 0, mz)] = 0.333f * (
      src[Index(1, 0, mz)] +
          src[Index(0, 1, mz)] +
          src[Index(0, 0, mz - 1)]);

  src[Index(0, my, mz)] = 0.333f * (
      src[Index(0, my - 1, mz)]
          + src[Index(1, my, mz)]
          + src[Index(0, my, mz - 1)]);

  src[Index(mx, my, mz)] = 0.333f * (
      src[Index(mx - 1, my, mz)]
          + src[Index(mx, my - 1, mz)]
          + src[Index(mx, my, mz - 1)]);

  src[Index(mx, 0, mz)] = 0.333f * (
      src[Index(mx - 1, 0, mz)]
          + src[Index(mx, 1, mz)]
          + src[Index(mx, 0, mz - 1)]);
}


/* ************************************************************************
 * **
 * ** Velocity Solver
 * **
 * ************************************************************************/
void GridFluidSimulator3D::VelocityStep(const std::vector<vec3f> &forces, float delta_t) {
  ApplyForces(forces, delta_t);
  DiffuseVelocity(delta_t);
  Project();
  AdvectVelocity(delta_t);
  Project();
}

/*
 * Apply all forces at the centre of each cell.
 * For each force, for each cell, compute the force and
 * then apply to the velocity
 * F = ma so
 * m = w.h.d.rho
 * a = F / m
 * v = v + (a * delta_t)
 */
void GridFluidSimulator3D::ApplyForces(const std::vector<vec3f> &forces, float delta_t) {
  auto inv_mass = 1.0f / (cell_size_.x * cell_size_.y * cell_size_.z * density_);
  for (auto cell_idx = 0; cell_idx < total_cells_; ++cell_idx) {
    auto accel = forces.at(cell_idx) * inv_mass * delta_t;

    velocity_x_.at(cell_idx) += accel.x;
    velocity_y_.at(cell_idx) += accel.y;
    velocity_z_.at(cell_idx) += accel.z;
  }
}

void GridFluidSimulator3D::AdvectVelocity(float delta_t) {
  std::vector<float> advected_vx(total_cells_, 0.0f);
  std::vector<float> advected_vy(total_cells_, 0.0f);
  std::vector<float> advected_vz(total_cells_, 0.0f);
  for (auto z = 1; z < num_cells_.z - 1; ++z) {
    for (auto y = 1; y < num_cells_.y - 1; ++y) {
      for (auto x = 1; x < num_cells_.x - 1; ++x) {
        advected_vx.at(Index(x, y, z)) =
            AdvectValue(velocity_x_, velocity_y_, velocity_z_,
                        velocity_x_, x, y, z, delta_t);
        advected_vy.at(Index(x, y, z)) =
            AdvectValue(velocity_x_, velocity_y_, velocity_z_,
                        velocity_y_, x, y, z, delta_t);
        advected_vz.at(Index(x, y, z)) =
            AdvectValue(velocity_x_, velocity_y_, velocity_z_,
                        velocity_z_, x, y, z, delta_t);
      }
    }
  }
  std::memcpy(velocity_x_.data(), advected_vx.data(), sizeof(float) * total_cells_);
  std::memcpy(velocity_y_.data(), advected_vy.data(), sizeof(float) * total_cells_);
  std::memcpy(velocity_z_.data(), advected_vz.data(), sizeof(float) * total_cells_);
}

void GridFluidSimulator3D::DiffuseVelocity(float delta_t) {
  Diffuse(velocity_x_, viscosity_, delta_t, true);
  Diffuse(velocity_y_, viscosity_, delta_t, true);
  Diffuse(velocity_z_, viscosity_, delta_t, true);
}

void GridFluidSimulator3D::Project() {
  std::vector<float> divergence(total_cells_);
  std::vector<float> pressure(total_cells_);

  auto idx_dz = num_cells_.x * num_cells_.y;
  auto idx_dy = num_cells_.x;

  for (auto x = 1; x < num_cells_.x - 1; ++x) {
    for (auto y = 1; y < num_cells_.y - 1; ++y) {
      for (auto z = 1; z < num_cells_.z - 1; ++z) {
        auto idx = Index(x, y, z);

        divergence.at(idx) = -0.5f *
            (/*cell_size_.x */ (velocity_x_.at(idx + 1) - velocity_x_.at(idx - 1))) +
            (/*cell_size_.y */ (velocity_y_.at(idx + idx_dy) - velocity_y_.at(idx - idx_dy))) +
            (/*cell_size_.z */ (velocity_z_.at(idx + idx_dz) - velocity_z_.at(idx - idx_dz)));
        pressure.at(idx) = 0.0f;
      }
    }
  }
  SetBoundary(pressure, 1.0f);
  SetBoundary(divergence, 1.0f);

  for (auto iter = 0; iter < NUM_GS_ITERS; ++iter) {

    for (auto x = 1; x < num_cells_.x - 1; ++x) {
      for (auto y = 1; y < num_cells_.y - 1; ++y) {
        for (auto z = 1; z < num_cells_.z - 1; ++z) {
          auto idx = Index(x, y, z);
          pressure.at(idx) = (divergence.at(idx)
              + pressure.at(idx - 1) + pressure.at(idx + 1)
              + pressure.at(idx - idx_dy) + pressure.at(idx + idx_dy)
              + pressure.at(idx - idx_dz) + pressure.at(idx + idx_dz)) / 6.0f;
        }
      }
    }
    SetBoundary(pressure, 1.0f);
  }

  for (auto x = 1; x < num_cells_.x - 1; ++x) {
    for (auto y = 1; y < num_cells_.y - 1; ++y) {
      for (auto z = 1; z < num_cells_.z - 1; ++z) {
        auto idx = Index(x, y, z);
        velocity_x_.at(idx) -= 0.5f * (pressure.at(idx + 1) - pressure.at(idx - 1)) / cell_size_.x;
        velocity_y_.at(idx) -= 0.5f * (pressure.at(idx + idx_dy) - pressure.at(idx - idx_dy)) / cell_size_.y;
        velocity_z_.at(idx) -= 0.5f * (pressure.at(idx + idx_dz) - pressure.at(idx - idx_dz)) / cell_size_.z;
      }
    }
  }
  SetBoundary(velocity_x_, -1.0f);
  SetBoundary(velocity_y_, -1.0f);
  SetBoundary(velocity_z_, -1.0f);
}

/* ************************************************************************
 * **
 * ** Scalar Solver
 * **
 * ************************************************************************/
void GridFluidSimulator3D::ScalarStep(const std::vector<float> &sources, float delta_t) {
  ApplySources(sources, delta_t);
  AdvectScalar(delta_t);
  DiffuseScalar(delta_t);
  Dissipate(delta_t);
}

void GridFluidSimulator3D::ApplySources(const std::vector<float> &sources, float delta_t) {
  for (auto cell_idx = 0; cell_idx < total_cells_; ++cell_idx) {
    content_.at(cell_idx) += (sources.at(cell_idx) * delta_t);
  }
}

void GridFluidSimulator3D::AdvectScalar(float delta_t) {
  std::vector<float> advected_content(total_cells_, 0.0f);
  for (auto z = 1; z < num_cells_.z - 1; ++z) {
    for (auto y = 1; y < num_cells_.y - 1; ++y) {
      for (auto x = 1; x < num_cells_.x - 1; ++x) {
        advected_content.at(Index(x, y, z)) =
            AdvectValue(velocity_x_, velocity_y_, velocity_z_,
                        content_, x, y, z, delta_t);
      }
    }
  }
  std::memcpy(content_.data(), advected_content.data(), sizeof(float) * total_cells_);
}

void GridFluidSimulator3D::DiffuseScalar(float delta_t) {
  Diffuse(content_, diffusion_rate_, delta_t, false);
}

void GridFluidSimulator3D::Dissipate(float delta_t) {
  for (auto &c : content_) {
    c = c / (1.0f + delta_t * dissipation_rate_);
  }
}

/* ************************************************************************
 * **
 * ** Main simulation step
 * **
 * ************************************************************************/
void GridFluidSimulator3D::Simulate(const std::vector<float> &sources,
                                    const std::vector<vec3f> &forces,
                                    float delta_t
) {
  if (sources.size() != total_cells_) {
    spdlog::error("Invalid number of sources, expected {} but got {}", total_cells_, sources.size());
    return;
  }
  if (forces.size() != total_cells_) {
    spdlog::error("Invalid number of forces, expected {} but got {}", total_cells_, forces.size());
    return;
  }
  VelocityStep(forces, delta_t);
  ScalarStep(sources, delta_t);
}