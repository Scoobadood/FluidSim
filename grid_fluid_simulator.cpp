#include "grid_fluid_simulator.h"
#include <QThread>
#include <cmath>
#include <iostream>

const float FLOW_RATE = 0.1f;

GridFluidSimulator::GridFluidSimulator(uint32_t width,      //
                                       uint32_t height,     //
                                       float delta_t,       //
                                       float diffusion_rate //
                                       )                    //
    : FluidSimulator2D{width, height}                       //
    , delta_t_{delta_t}                                     //
    , diffusion_rate_{diffusion_rate}                       //
{
    Initialise();
}

void GridFluidSimulator::InitialiseDensity()
{
    // Initialise with a blob in the middle
    auto rad = dim_x_ / 4.0f;
    auto rad2 = rad * rad;
    float cx = dim_x_ / 2.0f;
    float cy = dim_y_ / 2.0f;
    for (int y = 0; y < dim_y_; y++) {
        for (int x = 0; x < dim_x_; x++) {
            float dx = x + 0.5f - cx;
            float dy = y + 0.5f - cy;
            float d2 = dx * dx + dy * dy;
            density_.at(Index(x, y)) = (d2 < rad2) ? 1.0f : 0.0f;
        }
    }
}

void GridFluidSimulator::InitialiseVelocity()
{
    // Initialise a cosine wave of velocity
    float cx = dim_x_ * 0.5f;
    float cy = dim_y_ * 0.5f;
    for (int y = 0; y < dim_y_; y++) {
        for (int x = 0; x < dim_x_; x++) {
            float angle = ((x - cx) / dim_x_) * M_PI;
            float len = (1.0f - (std::fabsf(cy - y) / dim_y_)) * 0.5f;
            auto vx = len * std::cosf(angle);
            auto vy = len * std::sinf(angle);
            velocity_x_.at(Index(x, y)) = vx;
            velocity_y_.at(Index(x, y)) = vy;
        }
    }
}

void GridFluidSimulator::Diffuse(std::vector<float>& target_density)
{
    // S is the target density for each cell. It's the average of the 4-neighbours
    std::vector<float> s(num_cells_);
    // Compute S
    for (auto y = 1; y < dim_y_ - 1; ++y) {
        for (auto x = 1; x < dim_x_ - 1; ++x) {
            s.at(Index(x, y)) = 0.25f
                                * (density_.at(Index(x - 1, y)) + density_.at(Index(x + 1, y))
                                   + density_.at(Index(x, y - 1)) + density_.at(Index(x, y + 1)));
        }
    }

    // Now compute the diffusion by using the time step and diffusion parameters
    // This is (a) Linear and not good
    // (b) Unstable, if delta_t * diffusion_rate_ exceeds 1.0f values can go nuts.
    for (auto y = 1; y < dim_y_ - 1; ++y) {
        for (auto x = 1; x < dim_x_ - 1; ++x) {
            auto idx = Index(x, y);
            target_density.at(idx) = density_.at(idx)
                                     + (delta_t_ * diffusion_rate_ * (s.at(idx) - density_.at(idx)));
        }
    }
}

void GridFluidSimulator::Simulate()
{
    std::vector<float> target_density(num_cells_);
    Diffuse(target_density);

    // Update the density
    for (auto i = 0; i < density_.size(); ++i) {
        density_.at(i) = target_density.at(i);
    }
}
