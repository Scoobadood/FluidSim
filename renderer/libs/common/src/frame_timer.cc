
#include "frame_timer.h"

#include <chrono>

FrameTimer::FrameTimer() //
{
  last_time_s_ = std::chrono::high_resolution_clock::now();
}

float FrameTimer::ElapsedTime() {
  auto now = std::chrono::high_resolution_clock::now();
  std::chrono::duration<float> delta_time = now - last_time_s_;
  last_time_s_ = now;
  float dts = delta_time.count();
  return dts;
}

