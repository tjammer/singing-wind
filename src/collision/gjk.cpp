#include "gjk.h"
#include "convex_shape.h"
#include "transform.h"
#include "simplex.h"
#include "epa.h"
#include "vec_math.h"

const float EPSILON = std::numeric_limits<float>::epsilon();

MinkowskiDiff::MinkowskiDiff(const ConvexShape& c1,
                             const Transform& t1,
                             const ConvexShape& c2,
                             const Transform& t2)
  : m_c1(c1)
  , m_t1(t1)
  , m_c2(c2)
  , m_t2(t2)
{}

WVec
MinkowskiDiff::support(const WVec& v) const
{
  return m_c1.support(v, m_t1) - m_c2.support(-v, m_t2);
}

WVec
initial_direction(const ConvexShape& c1,
                  const Transform& t1,
                  const ConvexShape& c2,
                  const Transform& t2)
{
  return transformed(t2, c2.center()) - transformed(t1, c1.center());
}

bool
check_simplex(std::vector<WVec>& simplex, WVec& dir)
{
  WVec a = simplex.back();

  // this is the same as a.to(ORIGIN);
  WVec ao = -a;

  // check to see what type of simplex we have
  if (simplex.size() == 3) {
    // then we have a triangle
    WVec b = simplex[1];
    WVec c = simplex[0];

    // get the edges
    WVec ab = b - a;
    WVec ac = c - a;

    // get the edge normals
    WVec ab_perp = w_triple_prod(ac, ab, ab);
    WVec ac_perp = w_triple_prod(ab, ac, ac);

    // see where the origin is at
    float origin = dot(ac_perp, ao);

    if (origin >= 0.0) {
      // the origin lies on the right side of A->C
      // because of the condition for the gjk loop to continue the origin
      // must lie between A and C so remove B and set the
      // new search direction to A->C perpendicular vector
      simplex.erase(simplex.begin() + 1);

      // this used to be direction.set(Vector.tripleProduct(ac, ao, ac));
      // but was changed since the origin may lie on the segment created
      // by a -> c in which case would produce a zero vector normal
      // calculating ac's normal using b is more robust
      dir = ac_perp;
    } else {
      float abLocation = dot(ab_perp, ao);

      // the origin lies on the left side of A->C
      if (abLocation < 0.0) {
        // the origin lies on the right side of A->B and therefore in the
        // triangle, we have an intersection
        return true;
      } else {
        // the origin lies between A and B so remove C and set the
        // search direction to A->B perpendicular vector
        simplex.erase(simplex.begin());

        // this used to be direction.set(Vector.tripleProduct(ab, ao, ab));
        // but was changed since the origin may lie on the segment created
        // by a -> b in which case would produce a zero vector normal
        // calculating ab's normal using c is more robust
        dir = ab_perp;
      }
    }
  } else {
    // get the b point
    WVec b = simplex[0];
    WVec ab = b - a;

    // otherwise we have 2 points (line segment)
    // because of the condition for the gjk loop to continue the origin
    // must lie in between A and B, so keep both points in the simplex and
    // set the direction to the perp of the line segment towards the origin
    dir = w_triple_prod(ab, ao, ab);

    // check for degenerate cases where the origin lies on the segment
    // created by a -> b which will yield a zero edge normal
    if (w_dot(dir, dir) <= EPSILON) {
      // in this case just choose either normal (left or right)
      dir = w_perp(ab);
    }
  }

  return false;
}

bool
gjk_detect(const MinkowskiDiff& mdiff, std::vector<WVec>& simplex, WVec& dir)
{
  if (w_magnitude(dir) == 0) {
    dir = WVec{ 1, 0 };
  }

  simplex.push_back(mdiff.support(dir));

  if (w_dot(simplex[0], dir) <= 0) {
    return false;
  }

  dir *= -1.f;

  while (true) {
    simplex.push_back(mdiff.support(dir));
    auto last = simplex.back();

    if (w_dot(last, dir) <= 0) {
      return false;
    } else {
      if (check_simplex(simplex, dir)) {
        return true;
      }
    }
  }
}

ColResult
gjk_collide(const ConvexShape& c1,
            const Transform& t1,
            const ConvexShape& c2,
            const Transform& t2)
{
  ColResult result;
  std::vector<WVec> simplex;
  auto mdiff = MinkowskiDiff{ c1, t1, c2, t2 };
  auto dir = initial_direction(c1, t1, c2, t2);

  if (gjk_detect(mdiff, simplex, dir)) {
    epa_solve(simplex, mdiff, result);
  }
  return result;
}
