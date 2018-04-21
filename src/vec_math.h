//
// Created by jammer on 17/05/17.
//

#ifndef SINGING_WIND_WVECMATH_H
#define SINGING_WIND_WVECMATH_H

#include "wind_defs.h"
#include <cmath>
#include <deque>

inline float
w_magnitude(const WVec& v)
{
  return glm::length(v);
}

inline WVec
w_normalize(const WVec& v)
{
  auto mag = w_magnitude(v);
  if (mag != 0) {
    return v / mag;
  }
  return v;
}

inline float
w_dot(const WVec& a, const WVec& b)
{
  return glm::dot(a, b);
}

inline float
w_cross(const WVec& a, const WVec& b)
{
  return a.x * b.y - a.y * b.x;
}

inline WVec
w_triple_prod(const WVec& a, const WVec& b, const WVec& c)
{
  return b * w_dot(a, c) - c * w_dot(a, b);
}

inline WVec
w_slide(const WVec& vec, const WVec& normal)
{
  return vec - normal * w_dot(vec, normal);
}

inline float
w_angle_to_vec(const WVec& v, const WVec& to)
{
  return atan2(w_cross(v, to), w_dot(v, to));
}

inline WVec
w_rotated(const WVec& v, float angle)
{
  float v_angle = atan2(v.y, v.x);
  return WVec(cos(v_angle + angle), sin(v_angle + angle)) * w_magnitude(v);
}

inline WVec
w_tangent(const WVec& v)
{
  return WVec(v.y, -v.x);
}

inline WVec
w_reflect(const WVec& v, const WVec& n)
{
  return v - 2.0f * w_dot(v, n) * n;
}

#endif // SINGING_WIND_WVECMATH_H
