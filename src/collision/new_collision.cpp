#include "new_collision.h"

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

  if (tbar0 > 0 && tbar0 < 1 && tbar1 > 0 && tbar1 < 1) {
    return 0;
  } else if (ttilde0 > 0 && ttilde0 < 1) {
    if (tbar1 <= 0) {
      return w_dot(dperp0, delta) / w_magnitude(a.d);
    } else if (tbar1 >= 1) {
      return w_dot(dperp0, delta - b.d) / w_magnitude(a.d);
    }
  } else if (ttilde1 > 0 && ttilde1 < 1) {
    if (tbar0 <= 0) {
      return w_dot(dperp1, delta) / w_magnitude(b.d);
    } else if (tbar0 >= 1) {
      return w_dot(dperp1, delta + a.d) / w_magnitude(b.d);
    }
  } else if (ttilde0 <= 0 && ttilde1 <= 0) {
    return w_magnitude(delta);
  } else if (ttilde0 >= 1 && ttilde1 <= 0) {
    return w_magnitude(delta + a.d);
  } else if (ttilde0 <= 0 && ttilde1 >= 1) {
    return w_magnitude(delta - b.d);
  } else if (ttilde0 >= 1 && ttilde1 >= 1) {
    return w_magnitude(delta + a.d - b.d);
  }
  assert(false);
  return 0;
}
