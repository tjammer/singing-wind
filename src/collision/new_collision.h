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
segment_to_segment(const LineSegment& a, const LineSegment& b);

#endif /* NEW_COLLISION_H */
