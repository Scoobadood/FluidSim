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
    // Initialise a spiral
    /*
     * fx   |   fy   |   vx  |  vy
     * -----+--------+-------+-----
     *   0  |   .01  |  0.0  | 0.5
     *   0  |  -.01  |  0.0  |-0.5
     *   0  |   .5   |  0.0  | 0.0
     *   0  |  -.5   |  0.0  |-0.0
     */
    float cx = dim_x_ * 0.5f;
    float cy = dim_y_ * 0.5f;
    for (int y = 0; y < dim_y_; y++) {
        for (int x = 0; x < dim_x_; x++) {
            auto fx = 4.f * (x - cx) / dim_x_;
            auto fy = 4.f * (y - cy) / dim_y_;
            velocity_x_.at(Index(x, y)) = fx - fy - fx * (fx * fx + fy * fy);
            velocity_y_.at(Index(x, y)) = fx + fy - fy * (fx * fx + fy * fy);
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

float GridFluidSimulator::AdvectValue(const std::vector<float>& source_data,
                                      uint32_t x,
                                      uint32_t y) const
{
    auto idx = Index(x, y);

    // Get the velocity for this cell
    auto vx = velocity_x_.at(idx);
    auto vy = velocity_y_.at(idx);

    // Get the source point for that flow
    auto source_x = x - vx * delta_t_;
    auto source_y = y - vy * delta_t_;

    // Get the base coord
    auto base_x = std::floorf(source_x);
    auto base_y = std::floorf(source_y);

    // And the fractional offset
    auto frac_x = source_x - base_x;
    auto frac_y = source_y - base_y;

    // Interpolate top and bottom
    auto top_lerp = Lerp(source_data.at(Index(base_x, base_y)),
                         source_data.at(Index(base_x + 1, base_y)),
                         frac_x);
    auto btm_lerp = Lerp(source_data.at(Index(base_x, base_y + 1)),
                         source_data.at(Index(base_x + 1, base_y + 1)),
                         frac_x);
    // interpolate top to bottom
    return Lerp(top_lerp, btm_lerp, frac_y);
}

void GridFluidSimulator::AdvectDensity(std::vector<float>& target_density)
{
    std::vector<float> advected_density(num_cells_);

    for (auto y = 1; y < dim_y_ - 1; ++y) {
        for (auto x = 1; x < dim_x_ - 1; ++x) {
            advected_density.at(Index(x, y)) = AdvectValue(target_density, x, y);
        }
    }
    std::memcpy(target_density.data(), advected_density.data(), num_cells_ * sizeof(float));
}

void GridFluidSimulator::AdvectVelocity()
{
    std::vector<float> advected_velocity_x(num_cells_);
    std::vector<float> advected_velocity_y(num_cells_);

    for (auto y = 1; y < dim_y_ - 1; ++y) {
        for (auto x = 1; x < dim_x_ - 1; ++x) {
            auto idx = Index(x, y);
            advected_velocity_x.at(idx) = AdvectValue(velocity_x_, x, y);
            advected_velocity_y.at(idx) = AdvectValue(velocity_y_, x, y);
        }
    }
    std::memcpy(velocity_x_.data(), advected_velocity_x.data(), num_cells_ * sizeof(float));
    std::memcpy(velocity_y_.data(), advected_velocity_y.data(), num_cells_ * sizeof(float));
}

void GridFluidSimulator::Simulate()
{
    std::vector<float> target_density(num_cells_);
    Diffuse(target_density);
    AdvectDensity(target_density);
    AdvectVelocity();

    // Update the density
    for (auto i = 0; i < density_.size(); ++i) {
        density_.at(i) = target_density.at(i);
    }
}
