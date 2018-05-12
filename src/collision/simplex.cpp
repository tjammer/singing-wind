#include "simplex.h"
#include "vec_math.h"

SimplexEdge::SimplexEdge(const WVec& p1, const WVec& p2, const int winding)
  : m_p1(p1)
  , m_p2(p2)
{
  auto dir = w_normalize(m_p2 - m_p1);

  if (winding < 0) {
    m_normal = w_perp(dir);
  } else {
    m_normal = -w_perp(dir);
  }

  m_dist = w_dot(p1, m_normal);
}

int
calc_winding(const std::vector<WVec>& simplex)
{
  auto size = simplex.size();
  for (unsigned int i = 0; i < size; ++i) {
    int j = i + 1 == size ? 0 : i + 1;

    auto a = simplex[i];
    auto b = simplex[j];

    float axb = w_cross(a, b);
    if (axb > 0) {
      return 1;
    } else if (axb < 0) {
      return -1;
    }
  }

  return 0;
}

Simplex::Simplex(const std::vector<WVec>& simplex)
  : m_winding(calc_winding(simplex))
{
  size_t size = simplex.size();
  for (unsigned int i = 0; i < size; i++) {
    int j = i + 1 == size ? 0 : i + 1;

    auto a = simplex[i];
    auto b = simplex[j];
    m_queue.push(SimplexEdge(a, b, m_winding));
  }
}

void
Simplex::add(const WVec& v)
{
  auto edge = m_queue.top();
  m_queue.pop();

  m_queue.push(SimplexEdge(edge.p1(), v, m_winding));
  m_queue.push(SimplexEdge(v, edge.p2(), m_winding));
}

SimplexEdge
Simplex::find_closest_edge() const
{
  return m_queue.top();
}
