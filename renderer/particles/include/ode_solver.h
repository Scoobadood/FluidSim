//
// Created by Dave Durbin on 5/1/2024.
//

#ifndef FLUIDSIM_ODE_SOLVER_H
#define FLUIDSIM_ODE_SOLVER_H

#include "particle_system.h"

void euler_solve(ParticleSystem& ps, float delta_t);
#endif //FLUIDSIM_ODE_SOLVER_H
