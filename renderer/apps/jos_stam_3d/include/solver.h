//
// Created by Dave Durbin on 12/1/2024.
//

#ifndef SOLVER_H_
#define SOLVER_H_

#define IX(i,j) ((i)+(N+2)*(j))

void dens_step ( int N, float * x, float * x0, float * u, float * v, float diff, float dt );
void vel_step ( int N, float * u, float * v, float * u0, float * v0, float visc, float dt );



#endif //SOLVER_H_
