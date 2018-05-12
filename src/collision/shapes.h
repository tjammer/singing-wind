#ifndef SHAPES_H
#define SHAPES_H

#include "convex_shape.h"
#include <vector>

class Polygon : public ConvexShape
{
public:
  Polygon(std::initializer_list<WVec> vecs);
  void transform(const Transform& t) override;
  WVec support(const WVec& dir, const Transform& t) const override;
  WVec support(const WVec& dir) const override;
  WVec center() const override { return m_tcenter; }
  float radius() const override { return m_radius; }
  WVec xextends() const { return m_xext; }
  WVec yextends() const { return m_yext; }
  std::vector<WVec> draw_vertices(const Transform& t) const override;

private:
  std::vector<WVec> m_vecs;
  WVec m_center;
  float m_radius;
  WVec m_xext;
  WVec m_yext;
  // transformed
  WVec m_tcenter;
  std::vector<WVec> m_tvecs;
};

class Capsule : public ConvexShape
{
public:
  Capsule(float length, float radius);
  void transform(const Transform& t) override;
  WVec support(const WVec& dir, const Transform& t) const override;
  WVec support(const WVec& dir) const override;
  WVec center() const override { return m_tcenter; }
  float radius() const override { return m_radius; }
  std::vector<WVec> draw_vertices(const Transform& t) const override;

private:
  WVec m_a;
  WVec m_b;
  float m_radius;
  float m_capsule_radius;
  // transformed
  WVec m_ta;
  WVec m_tb;
  WVec m_tcenter;
};

#endif /* SHAPES_H */
