#ifndef FLUID_SIM_GRID_H
#define FLUID_SIM_GRID_H

#include <cstdint>
#include <vector>

class FluidGridSimulator
{
public:
    FluidGridSimulator(uint32_t width, uint32_t height);

    void Simulate();

    const std::vector<float> &Data() const;

    uint32_t Width() const;
    uint32_t Height() const;
    void Initialise();

private:
    uint32_t width_;
    uint32_t height_;
    std::vector<float> density_;
};

#endif // FLUID_SIM_GRID_H
