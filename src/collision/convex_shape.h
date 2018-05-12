#ifndef CONVEX_SHAPE_H
#define CONVEX_SHAPE_H

#include "w_vec.h"
#include <vector>
struct Transform;

class ConvexShape
{
public:
  virtual ~ConvexShape(){};
  virtual void transform(const Transform& t) = 0;
  virtual WVec support(const WVec& v, const Transform& t) const = 0;
  virtual WVec support(const WVec& dir) const = 0;
  virtual WVec center() const = 0;
  virtual float radius() const = 0;
  virtual std::vector<WVec> draw_vertices(const Transform& t) const = 0;
};

#endif /* CONVEX_SHAPE_H */
