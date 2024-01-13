#include "jos_stam_2d.h"
#include <memory>

#define SWAP(x0, x) {float *tmp=x0;x0=x;x=tmp;}

JosStam2D::JosStam2D(uint32_t N, float sz) {
  N_ = N;
  size_ = (N + 2) * (N + 2);
  u_ = new float[size_];
  v_ = new float[size_];
  dens_ = new float[size_];

  std::memset(u_, 0, size_ * sizeof(float));
  std::memset(v_, 0, size_ * sizeof(float));
  std::memset(dens_, 0, size_ * sizeof(float));
  sz_ = (sz / (float) N_);
}

JosStam2D::~JosStam2D() {
  delete[] u_;
  delete[] v_;
  delete[] dens_;
}

void JosStam2D::add_source(float *target, const float *source, float delta_t) const {
  for (auto i = 0; i < size_; i++) {
    target[i] += delta_t * source[i];
  }
}


void JosStam2D::diffuse(int boundary,
                        float *target_density,
                        const float *source_density,
                        float diffusion_rate,
                        float delta_t) {
  float a = delta_t * diffusion_rate * 1.0f / (sz_ * sz_);

  // Gauss Seidel solver
  for (auto k = 0; k < 20; k++) {

    for (auto x = 1; x <= N_; x++) {
      for (auto y = 1; y <= N_; y++) {

        target_density[IX(x, y)] =
            (source_density[IX(x, y)] + a
                * (target_density[IX(x - 1, y)] +
                    target_density[IX(x + 1, y)] +
                    target_density[IX(x, y - 1)] +
                    target_density[IX(x, y + 1)])) / (1 + 4 * a);
      }
    }
    set_bnd(boundary, target_density);
  }
}

void JosStam2D::advect(int b, float *d, const float *d0,
                       const float *velocity_x,
                       const float *velocity_y,
                       float delta_t) {

  int i, j, i0, j0, i1, j1;
  float x, y, s0, t0, s1, t1, dt0;

  dt0 = delta_t * N_;

  for (i = 1; i <= N_; i++) {
    for (j = 1; j <= N_; j++) {

      x = i - dt0 * velocity_x[IX(i, j)];
      y = j - dt0 * velocity_y[IX(i, j)];
      if (x < 0.5) x = 0.5;
      if (x > N_ + 0.5) x = N_ + 0.5;
      i0 = (int) x;
      i1 = i0 + 1;
      if (y < 0.5) y = 0.5;
      if (y > N_ + 0.5) y = N_ + 0.5;
      j0 = (int) y;
      j1 = j0 + 1;
      s1 = x - i0;
      s0 = 1 - s1;
      t1 = y - j0;
      t0 = 1 - t1;
      d[IX(i, j)] = s0 * (t0 * d0[IX(i0, j0)] + t1 * d0[IX(i0, j1)]) + s1 * (t0 * d0[IX(i1, j0)] + t1 * d0[IX(i1, j1)]);
    }
  }
  set_bnd(b, d);
}

void JosStam2D::velocity_step(float *force_x, float *force_y,
                              float viscosity, float delta_t) {

  add_source(u_, force_x, delta_t);
  add_source(v_, force_y, delta_t);

  auto u_prev = new float[size_];
  auto v_prev = new float[size_];
  std::memset(u_prev, 0, size_ * sizeof(float));
  std::memset(v_prev, 0, size_ * sizeof(float));


  diffuse(1, u_prev, u_, viscosity, delta_t);
  diffuse(2, v_prev, v_, viscosity, delta_t);
  project(u_, v_);

  SWAP (u_prev, u_);
  SWAP (v_prev, v_);
  advect(1, u_, u_prev, u_prev, v_prev, delta_t);
  advect(2, v_, v_prev, u_prev, v_prev, delta_t);
  project(u_, v_);

  delete[] u_prev;
  delete[] v_prev;
}

void JosStam2D::project(float *u, float *v) {
  auto p = new float[size_];
  auto div = new float[size_];

  for (auto i = 1; i <= N_; i++) {
    for (auto j = 1; j <= N_; j++) {
      div[IX(i, j)] = -0.5f * sz_ * (u[IX(i + 1, j)] - u[IX(i - 1, j)] +
          v[IX(i, j + 1)] - v[IX(i, j - 1)]);
      p[IX(i, j)] = 0;

    }
  }
  set_bnd(0, div);
  set_bnd(0, p);

  for (auto k = 0; k < 20; k++) {
    for (auto i = 1; i <= N_; i++) {
      for (auto j = 1; j <= N_; j++) {
        p[IX(i, j)] = (div[IX(i, j)] + p[IX(i - 1, j)] + p[IX(i + 1, j)] + p[IX(i, j - 1)] + p[IX(i, j + 1)]) / 4;
      }
    }
    set_bnd(0, p);
  }

  for (auto i = 1; i <= N_; i++) {
    for (auto j = 1; j <= N_; j++) {
      u[IX(i, j)] -= 0.5f * (p[IX(i + 1, j)] - p[IX(i - 1, j)]) / sz_;
      v[IX(i, j)] -= 0.5f * (p[IX(i, j + 1)] - p[IX(i, j - 1)]) / sz_;
    }
  }
  set_bnd(1, u);
  set_bnd(2, v);

  delete[] p;
  delete[] div;
}

void JosStam2D::set_bnd(int boundary, float *x) {
  for (auto i = 1; i <= N_; i++) {
    x[IX(0, i)] = boundary == 1 ? -x[IX(1, i)] : x[IX(1, i)];
    x[IX(N_ + 1, i)] = boundary == 1 ? -x[IX(N_, i)] : x[IX(N_, i)];
    x[IX(i, 0)] = boundary == 2 ? -x[IX(i, 1)] : x[IX(i, 1)];
    x[IX(i, N_ + 1)] = boundary == 2 ? -x[IX(i, N_)] : x[IX(i, N_)];
  }
  x[IX(0, 0)] = 0.5f * (x[IX(1, 0)] + x[IX(0, 1)]);
  x[IX(0, N_ + 1)] = 0.5f * (x[IX(1, N_ + 1)] + x[IX(0, N_)]);
  x[IX(N_ + 1, 0)] = 0.5f * (x[IX(N_, 0)] + x[IX(N_ + 1, 1)]);
  x[IX(N_ + 1, N_ + 1)] = 0.5f * (x[IX(N_, N_ + 1)] + x[IX(N_ + 1, N_)]);
}

void JosStam2D::density_step(float *source, float diffusion_rate, float delta_t) {

  add_source(dens_, source, delta_t);

  auto temp_density = new float[size_];
  std::memset(temp_density, 0, size_ * sizeof(float));

  diffuse(0, temp_density, dens_, diffusion_rate, delta_t);
  advect(0, dens_, temp_density,u_, v_, delta_t);

  delete[] temp_density;
}

void JosStam2D::simulate(float delta_t,
                         float *force_x,
                         float *force_y,
                         float *source,
                         float viscosity,
                         float diffusion_rate) {

  velocity_step(force_x, force_y, viscosity, delta_t);
  density_step(source, diffusion_rate, delta_t);
}