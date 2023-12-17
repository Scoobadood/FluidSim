#ifndef FLUID_GRID_H
#define FLUID_GRID_H

#include <QObject>
#include <cstdint>
#include <vector>

class FluidGrid : public QObject {
    Q_OBJECT

public:
    FluidGrid(uint32_t width, uint32_t height)//
        : width_{width} //
        , height_{height} //
        , x_{width / 2} //
        , y_{height / 2} //
        , dx_{0.3f} //
        , dy_{0.1f} //
        , cum_dx_{0} //
        , cum_dy_{0} //
    {
        width_ = width;
        height_ = height;
        density_.resize(width * height, 0);
    }

    void Simulate();


    uint32_t Width() const;
    uint32_t Height() const;
    uint32_t X() const {return x_;};
    uint32_t Y() const {return y_;};

private:
    uint32_t width_;
    uint32_t height_;
    std::vector<uint8_t> density_;
    uint32_t x_;
    uint32_t y_;
    float dx_;
    float dy_;
    float cum_dx_;
    float cum_dy_;
};

#endif // FLUID_GRID_H
