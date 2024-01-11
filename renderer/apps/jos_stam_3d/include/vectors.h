#ifndef FLUIDSIM_RENDERER_APPS_JOS_STAM_3D_INCLUDE_VECTORS_H_
#define FLUIDSIM_RENDERER_APPS_JOS_STAM_3D_INCLUDE_VECTORS_H_

#include <cstdint>
template<std::uint32_t D, typename T>
struct vec;

template<typename T>
struct vec<3, T> {
  T x;
  T y;
  T z;

  vec(T x(), T y(), T z()) {}
  vec(T x, T y, T z)//
      : x{x}, y{y}, z{z} {}
};

using vec3f = vec<3, float>;
using vec3u = vec<3, uint32_t>;

template<typename T>
vec<3, T> operator*(const vec<3, T> &lhs, float rhs) {
  return vec<3, T>(lhs.x * rhs, lhs.y * rhs, lhs.z * rhs);
}
template<typename T>
vec<3, T> operator-(const vec<3, T> &lhs, const vec<3, T> &rhs) {
  return vec<3, T>(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z);
}
#endif //FLUIDSIM_RENDERER_APPS_JOS_STAM_3D_INCLUDE_VECTORS_H_
