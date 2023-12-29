#ifndef FLUID_SIM_GRID_H
#define FLUID_SIM_GRID_H

#include "fluid_simulator.h"

#include <cstdint>
#include <vector>

class GridFluidSimulator : public FluidSimulator
{
public:
    GridFluidSimulator(uint32_t width, uint32_t height);

    void Simulate() override;

    const std::vector<float> &Density() const override;

    uint32_t Width() const;
    uint32_t Height() const;
    void Initialise();

private:
    const uint32_t width_;
    const uint32_t height_;
    std::vector<float> density_;
};

#endif // FLUID_SIM_GRID_H
