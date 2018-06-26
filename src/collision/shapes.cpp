#include "shapes.h"
#include "vec_math.h"
#include "transform.h"
#include <limits>

WVec
weighted_center(const std::vector<WVec>& vecs)
{
  // from wikipedia Centroid
  if (vecs.size() == 0) {
    assert(false);
  }

  float x = 0;
  float y = 0;
  float area = 0;

  for (unsigned int i = 0; i < vecs.size(); ++i) {
    auto& p0 = vecs[i];
    auto& p1 = vecs[(i + 1) % vecs.size()];

    float fac = p0.x * p1.y - p1.x * p0.y;
    x += (p0.x + p1.x) * fac;
    y += (p0.y + p1.y) * fac;

    area += fac * 0.5f;
  }
  x /= (6.f * area);
  y /= (6.f * area);
  return WVec{ x, y };
}

PolygonShape::PolygonShape(const std::vector<WVec>& vecs)
{
  m_vecs.insert(m_vecs.end(), vecs.begin(), vecs.end());
  m_tvecs.insert(m_tvecs.end(), vecs.begin(), vecs.end());

  m_center = weighted_center(m_vecs);
  m_tcenter = m_center;

  m_radius = -std::numeric_limits<float>::max();
  WVec xext{ 0, 0 };
  WVec yext{ 0, 0 };
  for (auto& v : m_vecs) {
    auto diff = v - m_center;
    auto squared_len = w_dot(diff, diff);
    if (squared_len > m_radius) {
      m_radius = squared_len;
    }

    if (diff.x > xext.x) {
      xext.x = diff.x;
    } else if (diff.x < -xext.y) {
      xext.y = -diff.x;
    }
    if (diff.y > yext.x) {
      yext.x = diff.y;
    } else if (diff.y < -yext.y) {
      yext.y = -diff.y;
    }
  }
  m_radius = sqrtf(m_radius);
  m_xext = xext;
  m_yext = yext;
}

void
PolygonShape::transform(const Transform& t)
{
  m_tcenter = transformed(t, m_center);
  for (unsigned int i = 0; i < m_vecs.size(); ++i) {
    m_tvecs[i] = transformed(t, m_vecs[i]);
  }
}

WVec
PolygonShape::support(const WVec& dir, const Transform& t) const
{
  auto local_dir = inversed(t, dir);

  auto proj = -std::numeric_limits<float>::max();
  WVec out;
  for (auto& v : m_vecs) {
    float dot = w_dot(local_dir, v);
    if (dot > proj) {
      proj = dot;
      out = v;
    }
  }
  return transformed(t, out);
}

WVec
PolygonShape::support(const WVec& dir) const
{
  auto proj = -std::numeric_limits<float>::max();
  WVec out;
  for (auto& v : m_tvecs) {
    float dot = w_dot(dir, v);
    if (dot > proj) {
      proj = dot;
      out = v;
    }
  }
  return out;
}

std::vector<WVec>
PolygonShape::draw_vertices(const Transform& t) const
{
  std::vector<WVec> out;

  for (const auto& v : m_vecs) {
    out.push_back(transformed(t, v));
  }
  return out;
}

Capsule::Capsule(float length, float radius)
  : m_a(WVec{ 0, length / 2.f })
  , m_b(WVec{ 0, -length / 2.f })
  , m_radius(length / 2.f + radius)
  , m_capsule_radius(radius)
  , m_ta(m_a)
  , m_tb(m_b)
  , m_tcenter(WVec{ 0, 0 })
{}

void
Capsule::transform(const Transform& t)
{
  m_ta = transformed(t, m_a);
  m_tb = transformed(t, m_b);
  m_tcenter = transformed(t, WVec{ 0, 0 });
}

WVec
Capsule::support(const WVec& dir, const Transform& t) const
{
  auto local_dir = inversed(t, dir);

  auto proj = -std::numeric_limits<float>::max();
  WVec out;
  for (auto& v : { m_a, m_b }) {
    float dot = w_dot(local_dir, v);
    if (dot > proj) {
      proj = dot;
      out = v;
    }
  }
  return transformed(t, out) + w_normalize(dir) * m_capsule_radius;
}

WVec
Capsule::support(const WVec& dir) const
{
  auto proj = -std::numeric_limits<float>::max();
  WVec out;
  for (auto& v : { m_ta, m_tb }) {
    float dot = w_dot(dir, v);
    if (dot > proj) {
      proj = dot;
      out = v;
    }
  }
  return out + w_normalize(dir) * m_capsule_radius;
}

std::vector<WVec>
Capsule::draw_vertices(const Transform& t) const
{
  const unsigned int return_ct = 16;
  std::vector<WVec> out;
  float angle = 4 * acos(0.f) / (float)return_ct;

  for (unsigned int i = 0; i < return_ct; ++i) {
    out.push_back(support(WVec{ sinf(i * angle), cosf(i * angle) }, t));
  }
  return out;
}
