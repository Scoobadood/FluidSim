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
    // Initialise target_density with current values because why not
    std::memcpy(target_density.data(), density_.data(), num_cells_ * sizeof(float));

    // Run four iterations of GS
    // Dn(x,y) = Dc(x,y) + (k*0.25*(Dn(x+1,y)+Dn(x-1,y)+Dn(x,y+1)+Dn(x,y-1)))/(1+k)

    auto factor = delta_t_ * diffusion_rate_;
    auto denom = 1.0f / (factor + 1.0f);
    for (auto iter = 0; iter < 4; ++iter) {
        for (auto y = 1; y < dim_y_ - 1; ++y) {
            for (auto x = 1; x < dim_x_ - 1; ++x) {
                auto idx = Index(x, y);
                auto left = target_density.at(idx - 1);
                auto rght = target_density.at(idx + 1);
                auto up = target_density.at(idx - dim_x_);
                auto down = target_density.at(idx + dim_x_);

                auto new_val = (density_.at(idx) + factor * 0.25f * (left + rght + up + down))
                               * denom;
                target_density.at(idx) = new_val;
            }
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
