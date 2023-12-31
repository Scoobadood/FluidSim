//
// Created by Dave Durbin on 31/12/2023.
//

#ifndef FLUIDSIM_JOS_STAM_SIMULATOR_2D_H
#define FLUIDSIM_JOS_STAM_SIMULATOR_2D_H


#include "fluid_simulator_2d.h"

class JosStamSimulator2D : public FluidSimulator2D {
public:
  JosStamSimulator2D(uint32_t dim_x, uint32_t dim_y);
};


#endif //FLUIDSIM_JOS_STAM_SIMULATOR_2D_H
