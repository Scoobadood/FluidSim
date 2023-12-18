#ifndef FLUID_SIM_GRID_H
#define FLUID_SIM_GRID_H

#include <cstdint>
#include <vector>

class FluidGridSimulator
{
public:
    FluidGridSimulator(uint32_t width, uint32_t height) //
        : width_{width}                                 //
        , height_{height}                               //
        , x_{(int32_t) width / 2}                       //
        , y_{(int32_t) height / 2}                      //
        , dx_{0.3f}                                     //
        , dy_{0.1f}                                     //
        , cum_dx_{0}                                    //
        , cum_dy_{0}                                    //
    {
        width_ = width;
        height_ = height;
        density_.resize(width * height, 0);
    }

    void Simulate();
    void RenderBall(uint8_t colour);

    std::vector<uint8_t> Data() const;

    uint32_t Width() const;
    uint32_t Height() const;

private:
    uint32_t width_;
    uint32_t height_;
    std::vector<uint8_t> density_;
    int32_t x_;
    int32_t y_;
    float dx_;
    float dy_;
    float cum_dx_;
    float cum_dy_;
};

#endif // FLUID_SIM_GRID_H
