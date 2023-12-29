#ifndef FLUID_SIMULATOR_H
#define FLUID_SIMULATOR_H

#include <vector>

class FluidSimulator
{
public:
    virtual void Simulate() = 0;
    virtual const std::vector<float>& Density() const = 0;
};

#endif // FLUID_SIMULATOR_H
