#include "epa.h"
#include "simplex.h"
#include "gjk.h"
#include "vec_math.h"

const int MAX_IT = 20;
const float EPSILON = std::numeric_limits<float>::epsilon() * 1000;

void
epa_solve(const std::vector<WVec>& simplex,
          const TransformedMinkowskiDiff& mdiff,
          ColResult& result)
{
  Simplex epa_simplex(simplex);
  SimplexEdge edge;
  WVec p;

  for (int i = 0; i < MAX_IT; ++i) {
    edge = epa_simplex.find_closest_edge();
    p = mdiff.support(edge.normal());

    float proj = w_dot(p, edge.normal());
    if ((proj - edge.dist()) < EPSILON) {
      result.normal = edge.normal();
      result.depth = proj;
      result.collides = true;
      return;
    }
    epa_simplex.add(p);
  }
}
