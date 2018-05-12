#include "transform.h"
#include "vec_math.h"

WVec
transformed(const Transform& t, const WVec& v)
{
  WVec out;
  float cosa = cosf(t.rotation);
  float sina = sinf(t.rotation);

  out.x = cosa * v.x + sina * v.y + t.position.x;
  out.y = -sina * v.x + cosa * v.y + t.position.y;

  return out;
}

WVec
inversed(const Transform& t, const WVec& v)
{
  WVec out;
  float cosa = cosf(t.rotation);
  float sina = sinf(t.rotation);

  out.x = cosa * v.x - sina * v.y;
  out.y = sina * v.x + cosa * v.y;

  return out;
}
