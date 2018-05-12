#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "w_vec.h"

struct Transform
{
  WVec position{ 0, 0 };
  float rotation{ 0.f };
  int direction{ 1 };
};

WVec
transformed(const Transform& t, const WVec& v);

WVec
inversed(const Transform& t, const WVec& v);

#endif /* TRANSFORM_H */
