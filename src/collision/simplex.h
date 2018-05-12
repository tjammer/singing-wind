#ifndef SIMPLEX_H
#define SIMPLEX_H

#include "w_vec.h"
#include <vector>
#include <queue>

class SimplexEdge
{
public:
  SimplexEdge(const WVec& p1, const WVec& p2, const int winding);
  SimplexEdge() {}
  float dist() const { return m_dist; }
  WVec p1() const { return m_p1; }
  WVec p2() const { return m_p2; }
  WVec normal() const { return m_normal; }

private:
  WVec m_p1;
  WVec m_p2;
  WVec m_normal;
  float m_dist;
};

struct _Dist
{
  bool operator()(const SimplexEdge& lhs, const SimplexEdge& rhs)
  {
    return lhs.dist() > rhs.dist();
  }
};

class Simplex
{
public:
  Simplex(const std::vector<WVec>& simplex);
  SimplexEdge find_closest_edge() const;
  void add(const WVec& v);

private:
  std::priority_queue<SimplexEdge, std::vector<SimplexEdge>, _Dist> m_queue;
  int m_winding;
};
#endif /* SIMPLEX_H */
