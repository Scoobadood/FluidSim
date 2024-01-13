#ifndef JOS_STAM_2D_H_
#define JOS_STAM_2D_H_

#include <cstdint>

class JosStam2D {
 public:
  explicit JosStam2D(uint32_t N, float sz);
  ~JosStam2D();
  float *dens_;
  void simulate(float dt, float *force_x, float *force_y, float *source, float viscosity, float diffusion_rate);

 private:
  void add_source(float *target, const float *source, float delta_t) const;
  void diffuse(int boundary, float *target_density, const float *source_density, float diffusion_rate, float delta_t);
  void advect(int b, float *d, const float *d0,
              const float *velocity_x,
              const float *velocity_y,
              float delta_t);
  void density_step(float *source, float diffusion_rate, float dt);
  void velocity_step(float *force_x, float *force_y, float viscosity, float delta_t);
  void set_bnd(int boundary, float *x);
  void project(float *u, float *v);
  inline uint32_t IX(uint32_t i, uint32_t j) const { return i + (N_ + 2) * j; }

  uint32_t N_;
  float *u_;
  float *v_;
  uint32_t size_;
  float sz_;
};
#endif //JOS_STAM_2D_H_
