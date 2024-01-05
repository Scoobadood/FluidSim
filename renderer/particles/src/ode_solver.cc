#include "particle_system.h"

void euler_solve(ParticleSystem& ps, float delta_t){
  auto derivative = ps.Derivative();
  auto state = ps.GetState();
  for( auto i=0; i<state.size(); ++i) {
    state[i] = state[i] + delta_t * derivative[i];
  }
  ps.SetState(state);
}
