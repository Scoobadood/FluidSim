#ifndef GRID_FLUID_SIMULATOR_H
#define GRID_FLUID_SIMULATOR_H

#include "fluid_simulator_2d.h"

#include <cstdint>
#include <vector>

class GridFluidSimulator : public FluidSimulator2D
{
public:
    GridFluidSimulator(uint32_t width, uint32_t height);

    void Simulate() override;
    uint32_t Width() const;
    uint32_t Height() const;
    void InitialiseDensity() override;
    void InitialiseVelocity() override;
};

#endif // GRID_FLUID_SIMULATOR_H
