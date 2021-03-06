//
// Created by jammer on 03/04/17.
//

#ifndef SINGING_WIND_COLLISION_H
#define SINGING_WIND_COLLISION_H

#include "WindDefs.h"
#include <vector>

class ColShape;
class ColTriangle;
struct StaticTriangle;
struct DynamicEntity;

struct ColResult
{
  bool collides = false;
  WVec normal = { 0, 0 };
  float depth = 0.f;
  int gjk_it = 0;
  int epa_it = 0;
};

struct RayCastResult
{
  bool hits = false;
  float hit_parameter = std::numeric_limits<float>::max();
  WVec hit_spot;
  WVec hit_normal;
  int its = 0;
  int innerits = 0;
};

std::ostream&
operator<<(std::ostream& os, const ColResult& result);

struct Edge
{
  float distance;
  WVec normal;
  int index;
};

struct SimplexVertex
{
  WVec p;  // vertex
  float u; // unnormalized baryonic coord

  SimplexVertex(WVec v)
    : p(v){};
  ~SimplexVertex() = default;
};

struct Simplex
{
  int count = 0;
  std::vector<WVec> verts = std::vector<WVec>(3);
  std::vector<float> barys = std::vector<float>(3);
  WVec dir;
  void solve(const WVec& x);
  bool add(const WVec& v, int index = 0);
  WVec get_closest() const;
  float denom;

private:
  void solve2(const WVec& x);
  void solve3(const WVec& x);
};

ColResult
static_collide(const ColShape& a, const ColShape& b);

RayCastResult
cast_ray_vs_shape(const WVec& p, const ColShape& b, const WVec& dir);
RayCastResult
cast_ray_vs_triangle(const WVec& p, const ColTriangle& b, const WVec& dir);

float
find_normal_epa(const ColShape& a,
                const ColShape& b,
                Simplex& s,
                WVec& normal,
                int&);

Edge
find_closest_edge(const Simplex& s);

WVec
find_directed_overlap(const ColResult& result, const WVec& direction);

#endif // SINGING_WIND_COLLISION_H
