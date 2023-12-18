#include "fluid_sim_grid.h"

uint32_t FluidGridSimulator::Width() const
{
    return width_;
}

uint32_t FluidGridSimulator::Height() const
{
    return height_;
}

std::vector<uint8_t> FluidGridSimulator::Data() const
{
    return density_;
}

void FluidGridSimulator::RenderBall(uint8_t colour)
{
    // Clear old data
    // ..xxxx..
    // .xxxxxx.
    // xxxxxxxx
    // xxxxxxxx
    bool ball[] = {
        0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 0, 0,
    };

    for (int yy = 0; yy < 8; ++yy) {
        if (y_ + yy >= height_)
            continue;
        for (int xx = 0; xx < 8; ++xx) {
            if (x_ + xx >= width_)
                continue;

            int idx = yy * 8 + xx;
            density_[(width_ * (y_ + yy)) + (x_ + xx)] = ball[idx] * colour;
        }
    }
}
void FluidGridSimulator::Simulate()
{
    RenderBall(0);
    cum_dx_ += dx_;
    cum_dy_ += dy_;
    auto delta_x = 0;
    auto delta_y = 0;

    if (dx_ > 0 && cum_dx_ >= 1.0f) {
        cum_dx_ -= 1.0f;
        delta_x = 1;
    } else if (dx_ < 0 && cum_dx_ <= -1.0f) {
        cum_dx_ += 1.0f;
        delta_x = -1;
    }
    x_ += delta_x;

    if (dy_ > 0 && cum_dy_ >= 1.0f) {
        cum_dy_ -= 1.0f;
        delta_y = 1;
    } else if (dy_ < 0 && cum_dy_ <= -1.0f) {
        cum_dy_ += 1.0f;
        delta_y = -1;
    }
    y_ += delta_y;

    if (x_ + 8 >= width_ || x_ < 0) {
        x_ -= delta_x;
        dx_ = -dx_;
    }
    if (y_ + 8 >= height_ || y_ < 0) {
        y_ -= delta_y;
        dy_ = -dy_;
    }

    // Add new data
    RenderBall(255);
}
