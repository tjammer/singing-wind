#ifndef SHAPES_H
#define SHAPES_H

#include "convex_shape.h"
#include <vector>

class Polygon : public ConvexShape
{
public:
  Polygon(std::initializer_list<WVec> vecs);
  WVec support(const WVec& dir, const Transform& t) const;
  WVec center() const { return m_center; }
  float radius() const { return m_radius; }
  WVec xextends() const { return m_xext; }
  WVec yextends() const { return m_yext; }
  std::vector<WVec> draw_vertices(const Transform& t) const;

private:
  std::vector<WVec> m_vecs;
  WVec m_center;
  float m_radius;
  WVec m_xext;
  WVec m_yext;
};

class Capsule : public ConvexShape
{
public:
  Capsule(float length, float radius);
  WVec support(const WVec& dir, const Transform& t) const;
  WVec center() const { return WVec{ 0, 0 }; }
  float radius() const { return m_radius; }
  std::vector<WVec> draw_vertices(const Transform& t) const;

private:
  WVec m_a;
  WVec m_b;
  float m_radius;
  float m_capsule_radius;
};

#endif /* SHAPES_H */
