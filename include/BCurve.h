//
// Created by jammer on 28/03/17.
//

#ifndef SINGING_WIND_BCURVE_H
#define SINGING_WIND_BCURVE_H

#include <WindDefs.h>
#include <vector>

struct BCurve
{
  const WVec& from;
  const WVec& ctrl_from;
  const WVec& ctrl_to;
  const WVec& to;

  /*WVec& operator[](int t) {
      t = t % 4;
      if (t == 0) return from;
      else if (t == 1) return ctrl_from;
      else if (t == 2) return ctrl_to;
      else return to;
  }*/
  const WVec& operator[](int t) const
  {
    t = t % 4;
    if (t == 0)
      return from;
    else if (t == 1)
      return ctrl_from;
    else if (t == 2)
      return ctrl_to;
    else
      return to;
  }

  /*void operator=(const BCurve &curve) {
      from = curve.from;
      ctrl_from = curve.ctrl_from;
      ctrl_to = curve.ctrl_to;
      to = curve.to;
  }*/

  std::vector<WVec> line_along_curve(float distance, float zoom) const;
  std::vector<WVec> points_along_curve(float distance) const;
  WVec eval(float t) const;
  WVec eval_perpendicular(float t) const;

private:
  WVec eval_tangent(float t) const;
  float approx_dist() const;
};

#endif // SINGING_WIND_BCURVE_H
