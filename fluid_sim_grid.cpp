#include "fluid_sim_grid.h"
#include <QThread>
#include <cmath>
#include <iostream>

const float FLOW_RATE = 0.1f;

FluidGridSimulator::FluidGridSimulator(uint32_t width, uint32_t height) //
    : width_{width}                                                     //
    , height_{height}                                                   //
{
    density_.resize(width * height, 0);
    Initialise();
}

void FluidGridSimulator::Initialise()
{
    // Initialise with a blob in the middle
    auto rad = width_ / 4.0f;
    auto rad2 = rad * rad;
    float cx = width_ / 2.0f;
    float cy = height_ / 2.0f;
    for (int y = 0; y < height_; y++) {
        for (int x = 0; x < width_; x++) {
            float dx = x + 0.5f - cx;
            float dy = y + 0.5f - cy;
            float d2 = dx * dx + dy * dy;
            density_.at(y * width_ + x) = (d2 < rad2) ? 1.0f : 0.0f;
        }
    }
}

uint32_t FluidGridSimulator::Width() const
{
    return width_;
}

uint32_t FluidGridSimulator::Height() const
{
    return height_;
}

const std::vector<float>& FluidGridSimulator::Data() const
{
    return density_;
}

inline uint32_t offset_index(uint32_t i, uint8_t j, uint32_t w)
{
    auto new_idx = i;
    if (j < 3)
        new_idx -= w;
    else if (j > 4)
        new_idx += w;
    if (j == 0 || j == 3 || j == 5)
        new_idx -= 1;
    else if (j == 2 || j == 4 || j == 7)
        new_idx += 1;
    return new_idx;
}

void FluidGridSimulator::Simulate()
{
    std::vector<float> flow(width_ * height_, 0.0f);

    std::vector<float> nbr_density(8);
    std::vector<float> desired_flow(8);

    // Make a first pass over the grid and compute the amount to offload and the share per neighour
    for (auto i = 0; i < density_.size(); ++i) {
        // Calculate the neighbour densities
        std::fill(nbr_density.begin(), nbr_density.end(), -1.0f);
        std::fill(desired_flow.begin(), desired_flow.end(), 0);

        for (auto j = 0; j < 8; ++j) {
            auto flow_idx = offset_index(i, j, width_);
            if (j < 3 && (i < width_))
                continue;
            if (j > 4 && (i / width_ == height_ - 1))
                continue;
            if ((j == 0 || j == 3 || j == 5) && (i % width_ == 0))
                continue;
            if ((j == 2 || j == 4 || j == 7) && (i % width_ == width_ - 1))
                continue;
            nbr_density[j] = density_.at(flow_idx);
        }

        // Work out the proportion of flow to each neighbour
        // We only flow to neighbours with a positive gradient
        auto curr_density = density_.at(i);
        float total_desired_flow = 0;
        for (auto j = 0; j < 8; j++) {
            auto d = nbr_density.at(j);
            if (d == -1)
                continue;
            if (d < curr_density) {
                desired_flow.at(j) = ((curr_density - d) * 0.5f);
                total_desired_flow += desired_flow.at(j);
            }
        }

        if (total_desired_flow > 1e-4) {
            // Work out the pro-rata amount of flow to each
            auto available_flow = curr_density * FLOW_RATE;
            auto flow_ratio = available_flow / total_desired_flow;

            for (auto j = 0; j < 8; j++) {
                if (nbr_density.at(j) == -1)
                    continue;
                auto flowed = flow_ratio * desired_flow.at(j);
                auto flow_idx = offset_index(i, j, width_);
                flow.at(flow_idx) += flowed;
                flow.at(i) -= flowed;
            }
        }
    }
    // Update the overall distribution
    for (auto i = 0; i < density_.size(); ++i) {
        density_.at(i) += flow.at(i);
    }
}
