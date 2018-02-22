//
// Created by jammer on 03/04/17.
//

#ifndef SINGING_WIND_COLSHAPE_H
#define SINGING_WIND_COLSHAPE_H

#include "WindDefs.h"
#include <array>

const float c_line_triangulate_split = 250.f;

struct ColResult;

enum class ColShapeName
{
  ColTriangle,
  ColCircle,
  ColCapsule,
  ColBase,
  ColPoint
};

class ColShape
{
public:
  virtual WVec get_support(const WVec& dir) const = 0;
  virtual float get_radius() const = 0;
  virtual ColResult collides(const ColShape& other) const;
  virtual void add_gfx_lines(const WTransform& transform) = 0;
  virtual void transform(const WTransform& transform) = 0;
  virtual void reset() = 0;

  // in local space
  WVec m_center;
  bool m_highlight = false;

  ColShapeName m_type = ColShapeName::ColBase;
};

class ColTriangle : public ColShape
{
public:
  WVec get_support(const WVec& dir) const override;
  float get_radius() const override { return m_radius; }
  virtual void add_gfx_lines(const WTransform& tf) override;
  virtual void transform(const WTransform& transform) override;
  virtual void reset() override;
  // for editor
  void set_radius(float r);
  void set_point(float l);
  float get_point() const { return -m_vertices[0].y; }

  ColTriangle(const WVec& p1, const WVec& p2, const WVec& p3);
  ColTriangle(const ColTriangle& tri, const WVec& v);
  ~ColTriangle() = default;

private:
  std::array<WVec, 3> m_vertices;
  float m_radius;
  std::array<WVec, 3> m_rest_vertices;
};

class ColCircle : public ColShape
{
public:
  WVec get_support(const WVec& dir) const override;
  float get_radius() const override { return m_radius; }
  virtual void add_gfx_lines(const WTransform& tf) override;
  virtual void transform(const WTransform& transform) override;
  virtual void reset() override;
  void set_radius(float r) { m_radius = r; }

  ColCircle(float radius);
  ~ColCircle() = default;

private:
  float m_radius;
};

class ColCapsule : public ColShape
{
public:
  WVec get_support(const WVec& dir) const override;
  float get_radius() const override { return m_radius; }
  virtual void add_gfx_lines(const WTransform& tf) override;
  virtual void transform(const WTransform& transform) override;
  virtual void reset() override;

  ColCapsule(float radius, float length);
  ~ColCapsule() = default;

  float get_capsule_radius() const { return m_capsule_radius; }
  void set_length(float length);
  void set_capsule_radius(float radius);

  float m_radius;
  float m_length;

private:
  float m_capsule_radius;
  WVec m_a;
  WVec m_b;
  const WVec m_rest_a;
  const WVec m_rest_b;
};

class ColPoint : public ColShape
{
public:
  WVec get_support(const WVec& dir) const override;
  float get_radius() const override { return 0; }
  virtual void add_gfx_lines(const WTransform&) override{};
  virtual void transform(const WTransform& transform) override;
  virtual void reset() override;

  ColPoint();
  ~ColPoint() = default;

  // in local space
  WVec m_center;
  bool m_highlight = false;
};

#endif // SINGING_WIND_COLSHAPE_H
