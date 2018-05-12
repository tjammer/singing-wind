#ifndef GJK_H
#define GJK_H

#include "w_vec.h"

class ConvexShape;
struct Transform;

struct ColResult
{
  bool collides{ false };
  WVec normal{ 0, 0 };
  float depth{ 0 };
};

class MinkowskiDiff
{
public:
  MinkowskiDiff(const ConvexShape& c1,
                const Transform& t1,
                const ConvexShape& c2,
                const Transform& t2);
  WVec support(const WVec& v) const;

private:
  const ConvexShape& m_c1;
  const Transform& m_t1;
  const ConvexShape& m_c2;
  const Transform& m_t2;
};

ColResult
gjk_collide(const ConvexShape& c1,
            const Transform& t1,
            const ConvexShape& c2,
            const Transform& t2);

#endif /* GJK_H */
