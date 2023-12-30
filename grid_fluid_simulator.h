#ifndef GRID_FLUID_SIMULATOR_H
#define GRID_FLUID_SIMULATOR_H

#include "fluid_simulator_2d.h"

#include <cstdint>
#include <vector>

class GridFluidSimulator : public FluidSimulator2D
{
public:
    GridFluidSimulator(uint32_t width,      //
                       uint32_t height,     //
                       float delta_t,       //
                       float diffusion_rate //
    );

    void Simulate() override;
    void InitialiseDensity() override;
    void InitialiseVelocity() override;

protected:
    void Diffuse(std::vector<float>& target_density);
    void SuppressDivergence();

  private:
    float AdvectValue(const std::vector<float>& source_data, uint32_t x, uint32_t y) const;
    void AdvectDensity(std::vector<float>& target_density);
    void AdvectVelocity();
    void ComputeDivergence(std::vector<float>& divergence) const;
    void ComputePressure(const std::vector<float>& divergence, std::vector<float>& pressure) const;
    void ComputeCurlField(const std::vector<float>& pressure,
			  std::vector<float>& curl_x,
			  std::vector<float>& curl_y) const;
    static inline float Lerp(float from, float to, float pct) { return from + pct * (to - from); }
    float delta_t_;
    float diffusion_rate_;
};

#endif // GRID_FLUID_SIMULATOR_H
