#ifndef FLUIDSIM_RENDERER_APPS_PARTICLE_INT_INCLUDE_FRAME_TIMER_H_
#define FLUIDSIM_RENDERER_APPS_PARTICLE_INT_INCLUDE_FRAME_TIMER_H_

#include <chrono>

class FrameTimer {
 public:
  FrameTimer();
  float ElapsedTime();

 private:
  std::chrono::steady_clock::time_point last_time_s_;
};
#endif //FLUIDSIM_RENDERER_APPS_PARTICLE_INT_INCLUDE_FRAME_TIMER_H_
