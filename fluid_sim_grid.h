#ifndef FLUID_SIM_GRID_H
#define FLUID_SIM_GRID_H

#include <cstdint>
#include <vector>

class FluidGridSimulator
{
public:
    FluidGridSimulator(uint32_t width, uint32_t height);

    void Simulate();

    std::vector<uint8_t> Data() const;

    uint32_t Width() const;
    uint32_t Height() const;

private:
    uint32_t width_;
    uint32_t height_;
    std::vector<float> density_;
    mutable std::vector<uint8_t> density_map_;
};

#endif // FLUID_SIM_GRID_H
