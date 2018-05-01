#ifndef NEW_COLLISION_H
#define NEW_COLLISION_H

#include "wind_defs.h"
#include "vec_math.h"

// P + td
struct LineSegment
{
  WVec P;
  WVec d;
};

float
segment_to_segment(const LineSegment& a, const LineSegment& b)
{
  assert(w_dot(a.d, w_perp(b.d)) != 0);

  auto delta = a.P - b.P;
  auto dperp1 = w_perp(b.d);
  auto dperp0 = w_perp(a.d);

  float tbar0 = w_dot(dperp1, delta) / w_dot(dperp1, a.d);
  float tbar1 = w_dot(dperp0, delta) / w_dot(dperp1, a.d);

  float ttilde0 = -w_dot(a.d, delta) / w_dot(a.d, a.d);
  float ttilde1 = -w_dot(b.d, delta) / w_dot(b.d, b.d);

  if (tbar0 > 0 && tbar0 < 1) {
    if (tbar0 > 0 && tbar0 < 1) {
      return 0;
    }
  } else if ()
}

#endif /* NEW_COLLISION_H */
