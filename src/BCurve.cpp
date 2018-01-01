//
// Created by jammer on 28/03/17.
//

#include "BCurve.h"
#include "Editor.h"
#include "WVecMath.h"

WVec
BCurve::eval(float t) const
{
  return powf(1 - t, 3) * from + 3 * powf(1 - t, 2) * t * ctrl_from +
         3 * (1 - t) * powf(t, 2) * ctrl_to + powf(t, 3) * to;
}

WVec
BCurve::eval_tangent(float t) const
{
  return -3.0f * (from * powf(1 - t, 2) - ctrl_from * (3 * t - 1) * (t - 1) +
                  t * (ctrl_to * (3 * t - 2) - to * t));
}

WVec
BCurve::eval_perpendicular(float t) const
{
  auto tangent = w_normalize(eval_tangent(t));
  return WVec(-tangent.y, tangent.x);
}

float
BCurve::approx_dist() const
{
  return w_magnitude(to - from);
}

std::vector<WVec>
BCurve::line_along_curve(float distance) const
{
  auto frac = fmax(floorf(approx_dist() / distance), 1.0f);
  std::vector<WVec> out;
  for (int i = 0; i < static_cast<int>(frac); ++i) {
    auto t = fmin(static_cast<float>(i) / frac, 1.0f);
    auto base = eval(t);
    out.push_back(base + eval_perpendicular(t) * c_line_size);
    out.push_back(base - eval_perpendicular(t) * c_line_size);
    t = fmin(static_cast<float>(i + 1) / frac, 1.0f);
    base = eval(t);
    out.push_back(base - eval_perpendicular(t) * c_line_size);
    out.push_back(base + eval_perpendicular(t) * c_line_size);
  }
  return out;
}

std::vector<WVec>
BCurve::points_along_curve(float distance) const
{
  auto frac = fmax(floorf(approx_dist() / distance), 1.0f);
  std::vector<WVec> out;
  for (int i = 0; i < static_cast<int>(frac); ++i) {
    out.push_back(eval(static_cast<float>(i) / frac));
  }
  return out;
}
