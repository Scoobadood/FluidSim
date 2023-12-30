#include "mike_ash_fluid_simulator.h"

MikeAshFluidSimulator::MikeAshFluidSimulator(uint32_t width,    //
                                             uint32_t height,   //
                                             uint32_t depth,    //
                                             float delta_t,     //
                                             float diffuseness, //
                                             float viscosity    //
)                  //
        : FluidSimulator()                                          //
        , width_{width}                                             //
        , height_{height}                                           //
        , depth_{depth}                                             //
        , delta_t_{delta_t}                                         //
        , diffuseness_{diffuseness}                                 //
        , viscosity_{viscosity}                                     //
{
  auto num_cells = width * height;
  s_.resize(num_cells, 0.0f);
  density_.resize(num_cells, 0.0f);
  velocity_x_.resize(num_cells, 0.0f);
  velocity_y_.resize(num_cells, 0.0f);
  velocity_z_.resize(num_cells, 0.0f);
  velocity_x_prev_.resize(num_cells, 0.0f);
  velocity_y_prev_.resize(num_cells, 0.0f);
  velocity_z_prev_.resize(num_cells, 0.0f);
}

const std::vector<float> &MikeAshFluidSimulator::Density() const {
  return density_;
}

void MikeAshFluidSimulator::AddDensity(uint32_t x, uint32_t y, uint32_t z, float amount) {
  density_.at(Index(x, y, z)) += amount;
}

void MikeAshFluidSimulator::AddVelocity(uint32_t x, //
                                        uint32_t y,
                                        uint32_t z,
                                        float amount_x,
                                        float amount_y,
                                        float amount_z) {
  auto idx = Index(x, y, z);
  velocity_x_.at(idx) += amount_x;
  velocity_y_.at(idx) += amount_y;
  velocity_z_.at(idx) += amount_z;
}

void MikeAshFluidSimulator::SetBoundary(uint32_t boundary, std::vector<float> &velocities) {
  // Handle top and bottom
  for (auto y = 1; y < height_ - 1; ++y) {
    for (auto x = 1; x < width_ - 1; ++x) {
      velocities[Index(x, y, 0)] = (boundary == 3) ? -velocities[Index(x, y, 1)]
                                                   : velocities[Index(x, y, 1)];
      velocities[Index(x, y, depth_ - 1)] = (boundary == 3)
                                            ? -velocities[Index(x, y, depth_ - 2)]
                                            : velocities[Index(x, y, depth_ - 2)];
    }
  }

  // Handle Front and back
  for (auto z = 1; z < depth_ - 1; ++z) {
    for (auto x = 1; x < width_ - 1; ++x) {
      velocities[Index(x, 0, z)] = boundary == 2 ? -velocities[Index(x, 1, z)]
                                                 : velocities[Index(x, 1, z)];
      velocities[Index(x, height_ - 1, z)] = boundary == 2
                                             ? -velocities[Index(x, height_ - 2, z)]
                                             : velocities[Index(x, height_ - 2, z)];
    }
  }

  // Left and right
  for (int z = 1; z < depth_ - 1; ++z) {
    for (int y = 1; y < height_ - 1; ++y) {
      velocities[Index(0, y, z)] = boundary == 1 ? -velocities[Index(1, y, z)]
                                                 : velocities[Index(1, y, z)];
      velocities[Index(width_ - 1, y, z)] = boundary == 1
                                            ? -velocities[Index(width_ - 2, y, z)]
                                            : velocities[Index(width_ - 2, y, z)];
    }
  }

  // Corners
  velocities[Index(0, 0, 0)] = (velocities[Index(1, 0, 0)] + //
                                velocities[Index(0, 1, 0)] + //
                                velocities[Index(0, 0, 1)])
                               * 0.333f;

  velocities[Index(0, height_ - 1, 0)] = (velocities[Index(1, height_ - 1, 0)] + //
                                          velocities[Index(0, height_ - 2, 0)] + //
                                          velocities[Index(0, height_ - 1, 1)])
                                         * 0.333f;

  velocities[Index(0, 0, depth_ - 1)] = (velocities[Index(1, 0, depth_ - 1)] + //
                                         velocities[Index(0, 1, depth_ - 1)] + //
                                         velocities[Index(0, 0, depth_ - 2)])
                                        * 0.333f;

  velocities[Index(0, height_ - 1, depth_ - 1)] = (velocities[Index(1, height_ - 1, depth_ - 1)]
                                                   + velocities[Index(0, height_ - 2, depth_ - 1)]
                                                   + velocities[Index(0, height_ - 1, depth_ - 2)])
                                                  * 0.333f;

  velocities[Index(width_ - 1, 0, 0)] = (velocities[Index(width_ - 2, 0, 0)] + //
                                         velocities[Index(width_ - 1, 1, 0)] + //
                                         velocities[Index(width_ - 1, 0, 1)])
                                        * 0.333f;

  velocities[Index(width_ - 1, height_ - 1, 0)] = (velocities[Index(width_ - 2, height_ - 1, 0)]
                                                   + //
                                                   velocities[Index(width_ - 1, height_ - 2, 0)]
                                                   + //
                                                   velocities[Index(width_ - 1, height_ - 1, 1)])
                                                  * 0.333f;

  velocities[Index(width_ - 1, 0, depth_ - 1)] = 0.33f
                                                 * (velocities[Index(width_ - 2, 0, depth_ - 1)]
                                                    + velocities[Index(width_ - 1, 1, depth_ - 1)]
                                                    + velocities[Index(width_ - 1, 0, depth_ - 2)]);
  velocities[Index(width_ - 1, height_ - 1, depth_ - 1)]
          = (velocities[Index(width_ - 2, height_ - 1, depth_ - 1)] + //
             velocities[Index(width_ - 1, height_ - 2, depth_ - 1)] + //
             velocities[Index(width_ - 1, height_ - 1, depth_ - 2)])
            * 0.333f;
}

void MikeAshFluidSimulator::LinearSolve(uint32_t boundary,
                                        std::vector<float> &next_values,
                                        const std::vector<float> &prev_values,
                                        float a,
                                        float c,
                                        uint32_t num_iterations,
                                        uint32_t dim_1,
                                        uint32_t dim_2) {
  float cRecip = 1.0f / c;
  for (auto iter_idx = 0; iter_idx < num_iterations; ++iter_idx) {
    // Consider each cell
    for (int z = 1; z < depth_ - 1; ++z) {
      for (auto y = 1; y < height_ - 1; ++y) {
        for (auto x = 1; x < width_ - 1; ++x) {
          auto curr_idx = Index(x, y, z);
          // Set the new value to the weighted old values
          next_values[curr_idx]
                  = cRecip
                    * (prev_values[curr_idx]
                       + a
                         * (next_values[curr_idx + 1] + next_values[curr_idx - 1]
                            + next_values[curr_idx + width_]
                            + next_values[curr_idx - width_]
                            + next_values[curr_idx + (width_ * height_)]
                            + next_values[curr_idx - (width_ * height_)]));
        }
      }
    }
    SetBoundary(boundary, next_values);
  }
}

void MikeAshFluidSimulator::Diffuse(uint32_t boundary,
                                    std::vector<float> &next_velocity,
                                    const std::vector<float> &prev_velocity,
                                    float diffusion_value,
                                    float delta_t,
                                    uint32_t num_iterations,
                                    uint32_t dim_1,
                                    uint32_t dim_2) {
  float a = delta_t * diffusion_value * (dim_1 - 2) * (dim_2 - 2);
  LinearSolve(boundary, next_velocity, prev_velocity, a, 1 + 6 * a, num_iterations, dim_1, dim_2);
}

void MikeAshFluidSimulator::Project(const std::vector<float> &prev_velocity_x,
                                    const std::vector<float> &prev_velocity_y,
                                    const std::vector<float> &prev_velocity_z,
                                    std::vector<float> &next_velocity_x,
                                    std::vector<float> &next_velocity_y,
                                    uint32_t num_neighbours) {}

void MikeAshFluidSimulator::Advect(uint32_t axis,
                                   std::vector<float> &next_velocity,
                                   const std::vector<float> &prev_velocity,
                                   const std::vector<float> &prev_velocity_x,
                                   const std::vector<float> &prev_velocity_y,
                                   const std::vector<float> &prev_velocity_z,
                                   uint32_t num_neighbours) {}

void MikeAshFluidSimulator::Simulate() {
  Diffuse(1, velocity_x_, velocity_x_prev_, viscosity_, delta_t_, 4, height_, depth_);
  Diffuse(2, velocity_y_, velocity_y_prev_, viscosity_, delta_t_, 4, width_, depth_);
  Diffuse(3, velocity_z_, velocity_z_prev_, viscosity_, delta_t_, 4, width_, height_);

  Project(velocity_x_prev_, velocity_y_prev_, velocity_z_prev_, velocity_x_, velocity_y_, 4);

  Advect(1,
         velocity_x_,
         velocity_x_prev_,
         velocity_x_prev_,
         velocity_y_prev_,
         velocity_z_prev_,
         delta_t_);
  Advect(2,
         velocity_y_,
         velocity_y_prev_,
         velocity_x_prev_,
         velocity_y_prev_,
         velocity_z_prev_,
         delta_t_);
  Advect(3,
         velocity_z_,
         velocity_z_prev_,
         velocity_x_prev_,
         velocity_y_prev_,
         velocity_z_prev_,
         delta_t_);

  Project(velocity_x_, velocity_y_, velocity_z_, velocity_x_prev_, velocity_y_prev_, 4);

  Diffuse(0, s_, density_, diffuseness_, delta_t_, 4, width_, height_);
  Advect(0, density_, s_, velocity_x_, velocity_y_, velocity_z_, delta_t_);
}
