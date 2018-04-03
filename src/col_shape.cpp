//
// Created by jammer on 03/04/17.
//

#include "col_shape.h"
#include "collision.h"
#include <renderer.h>
#include <vec_math.h>
#include <iostream>

ColTriangle::ColTriangle(const WVec& p1, const WVec& p2, const WVec& p3)
  : m_rest_vertices({ { p1, p2, p3 } })
{
  m_vertices[0] = p1;
  m_vertices[1] = p2;
  m_vertices[2] = p3;
  m_center = (p1 + p2 + p3) / 3.f;

  float radius = 0;
  for (const auto& v : m_vertices) {
    auto mag = w_magnitude(v - m_center);
    if (mag > radius) {
      radius = mag;
    }
  }
  m_radius = radius;

  m_type = ColShapeName::ColTriangle;
}

ColTriangle::ColTriangle(const ColTriangle& tri, const WVec& v)
  : ColTriangle(tri.m_vertices[0] + v,
                tri.m_vertices[1] + v,
                tri.m_vertices[2] + v)
{}

void
ColTriangle::add_gfx_lines(const WTransform& tf)
{
  WRenderer::set_mode(PLines);
  this->transform(tf);
  for (unsigned int i = 0; i < m_vertices.size(); ++i) {
    WRenderer::add_primitive_vertex(
      { { m_vertices[i].x, m_vertices[i].y }, { 1, 1, 1 } });
    WRenderer::add_primitive_vertex(
      { { m_vertices[(i + 1) % m_vertices.size()].x,
          m_vertices[(i + 1) % m_vertices.size()].y },
        { 1, 1, 1 } });
  }
  this->reset();
}

WVec
ColTriangle::get_support(const WVec& dir) const
{
  float max_dot = -std::numeric_limits<float>::max();
  WVec out(0, 0);

  for (const auto& vert : m_vertices) {
    auto d = w_dot(vert, dir);
    if (d > max_dot) {
      max_dot = d;
      out = vert;
    }
  }
  return out;
}

void
ColTriangle::transform(const WTransform& transform)
{
  m_center = WVec(transform * WVec3(m_center, 1));
  for (auto& vert : m_vertices) {
    vert = WVec(transform * WVec3(vert, 1));
  }
}

void
ColTriangle::reset()
{
  m_center = { 0, 0 };
  m_vertices = m_rest_vertices;
}

void
ColTriangle::set_radius(float r)
{
  float len = r / sqrtf(2);
  m_vertices[1] = { -len, len };
  m_vertices[2] = { len, len };
  m_rest_vertices[1] = m_vertices[1];
  m_rest_vertices[2] = m_vertices[2];

  float radius = 0;
  for (const auto& v : m_vertices) {
    auto mag = w_magnitude(v - m_center);
    if (mag > radius) {
      radius = mag;
    }
  }
  m_radius = radius;
}

void
ColTriangle::set_point(float l)
{
  m_vertices[0] = { 0, -l };
  m_rest_vertices[0] = m_vertices[0];

  float radius = 0;
  for (const auto& v : m_vertices) {
    auto mag = w_magnitude(v - m_center);
    if (mag > radius) {
      radius = mag;
    }
  }
  m_radius = radius;
}

void
ColCircle::add_gfx_lines(const WTransform& tf)
{
  WRenderer::set_mode(PLines);
  this->transform(tf);
  float angle = 4 * acos(0.f) / 32.f;
  for (unsigned int i = 0; i < 32; ++i) {
    WRenderer::add_primitive_vertex(
      { { m_center.x + sin(i * angle) * m_radius,
          m_center.y + cos(i * angle) * m_radius },
        { 0, 1, 0 } });
    WRenderer::add_primitive_vertex(
      { { m_center.x + sin((i + 1) * angle) * m_radius,
          m_center.y + cos((i + 1) * angle) * m_radius },
        { 0, 1, 0 } });
  }
  this->reset();
}

ColCircle::ColCircle(float radius)
  : m_radius(radius)
{
  m_center = WVec(0, 0);
  m_type = ColShapeName::ColCircle;
}

WVec
ColCircle::get_support(const WVec& dir) const
{
  float n = w_magnitude(dir); // dot(dir, dir);
  if (n != 0) {
    return m_center + dir * (m_radius / n);
  }
  return m_center + m_radius * WVec(0, 1);
}

void
ColCircle::transform(const WTransform& transform)
{
  m_center = WVec(transform * WVec3(m_center, 1.f));
}

void
ColCircle::reset()
{
  m_center = { 0, 0 };
}

ColResult
ColShape::collides(const ColShape& other) const
{
  return static_collide(*this, other);
}

ColCapsule::ColCapsule(float radius, float length)
  : m_length(length)
  , m_capsule_radius(radius)
  , m_rest_a(WVec(m_center.x, m_center.y + length / 2.f))
  , m_rest_b(WVec(m_center.x, m_center.y - length / 2.f))
{
  m_a = m_rest_a;
  m_b = m_rest_b;
  m_radius = length / 2.f + radius;
  m_type = ColShapeName::ColCapsule;
}

WVec
ColCapsule::get_support(const WVec& dir) const
{
  float n = w_magnitude(dir);
  float max_dot = -std::numeric_limits<float>::max();
  WVec out(-150, -150);

  for (const auto& v : { m_a, m_b }) {
    auto d = w_dot(v, dir);
    if (d > max_dot) {
      max_dot = d;
      out = v;
    }
  }
  if (n != 0) {
    return out + dir / n * m_capsule_radius;
  }
  return out + WVec(0, 1);
}

void
ColCapsule::transform(const WTransform& transform)
{
  m_center = WVec(transform * WVec3(m_center, 1));
  m_a = WVec(transform * WVec3(m_a, 1));
  m_b = WVec(transform * WVec3(m_b, 1));
}

void
ColCapsule::reset()
{
  m_center = { 0, 0 };
  m_a = m_rest_a;
  m_b = m_rest_b;
}

void
ColCapsule::add_gfx_lines(const WTransform& tf)
{
  WRenderer::set_mode(PLines);
  this->transform(tf);
  float angle = 4 * acos(0.f) / 32.f;
  for (unsigned int i = 0; i < 32; ++i) {
    WVec s1 = this->get_support(WVec(sin((i)*angle), cos((i)*angle)));
    WVec s2 =
      this->get_support(WVec(sin((i + 1) * angle), cos((i + 1) * angle)));
    WRenderer::add_primitive_vertex({ { s1.x, s1.y }, { 1, 1, 1 } });
    WRenderer::add_primitive_vertex({ { s2.x, s2.y }, { 1, 1, 1 } });
  }
  this->reset();
}

void
ColCapsule::set_length(float length)
{
  m_length = length;
  m_a = WVec(m_center.x, m_center.y + length / 2.f);
  m_b = WVec(m_center.x, m_center.y - length / 2.f);
  m_radius = length / 2.f + m_capsule_radius;
}

void
ColCapsule::set_capsule_radius(float radius)
{
  m_capsule_radius = radius;
  m_radius = m_length / 2.f + m_capsule_radius;
}

ColPoint::ColPoint()
{
  m_center = WVec(0, 0);
  m_type = ColShapeName::ColPoint;
}

WVec
ColPoint::get_support(const WVec&) const
{
  return m_center;
}

void
ColPoint::transform(const WTransform& transform)
{
  m_center = WVec(transform * WVec3(m_center, 1));
}

void
ColPoint::reset()
{
  m_center = { 0, 0 };
}
