#include "fluid_grid.h"

#include <QThread>

uint32_t FluidGrid::Width() const {
    return width_;
}

uint32_t FluidGrid::Height() const{
    return height_;
}

void FluidGrid::Simulate(){
    while( true) {
        cum_dx_ += dx_;
        cum_dy_ += dy_;
        auto delta_x = 0;
        auto delta_y = 0;

        if( dx_ > 0 && cum_dx_ >= 1.0f) {
            cum_dx_ -= 1.0f;
            delta_x = 1;
        }
        if ( dx_ < 0 && cum_dx_ <= -1.0f) {
            cum_dx_ += 1.0f;
            delta_x = -1;
        }
        x_ += delta_x;

        if( dy_ > 0 && cum_dy_ >= 1.0f) {
            cum_dy_ -= 1.0f;
            delta_y ++;
        }
        if ( dy_ < 0 && cum_dy_ <= -1.0f) {
            cum_dy_ += 1.0f;
            delta_y --;
        }
        y_ += delta_y;

        if( x_ >= width_ || x_ < 0) {
            x_ -= delta_x;
            dx_ = -dx_;
        }
        if( y_ >= height_ || y_ < 0) {
            y_ -= delta_y;
            dy_ = -dy_;
        }
        QThread::msleep(10);
    }
}
