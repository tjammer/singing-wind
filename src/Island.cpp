//
// Created by jammer on 28/03/17.
//

#include "Island.h"
#include "BCurve.h"
#include "WRenderer.h"

Island::Island(WVec pos, float spread)
{
  m_points.push_back(pos + WVec(-spread, spread));
  m_points.push_back(pos + WVec(spread, spread));
  m_points.push_back(pos + WVec(spread, -spread));
  m_points.push_back(pos + WVec(-spread, -spread));

  for (unsigned int i = 0; i < m_points.size(); ++i) {
    auto& from = m_points[i];
    auto& to = m_points[(i + 1) % m_points.size()];

    m_ctrl_points.push_back(from + (to - from) / 4.f);
    m_ctrl_points.push_back(to - (to - from) / 4.f);
  }
}

std::vector<WVec>
make_quad(const WVec& pos, float spread)
{
  std::vector<WVec> out;
  out.push_back(pos + WVec(-spread, spread));
  out.push_back(pos + WVec(spread, spread));
  out.push_back(pos + WVec(spread, -spread));
  out.push_back(pos + WVec(-spread, -spread));
  return out;
}

std::vector<PrimitiveVertex>
Island::get_points(float spread) const
{
  std::vector<PrimitiveVertex> out;
  for (const auto& point : m_points) {
    for (auto v : make_quad(point, spread)) {
      out.push_back({ { v.x, v.y }, { 1, 1, 1 } });
    }
  }
  return out;
}

std::vector<PrimitiveVertex>
Island::get_ctrl_points(float spread) const
{
  std::vector<PrimitiveVertex> out;
  for (const auto& point : m_ctrl_points) {
    for (auto v : make_quad(point, spread)) {
      out.push_back({ { v.x, v.y }, { 1, 1, 1 } });
    }
  }
  return out;
}

std::vector<PrimitiveVertex>
Island::get_curves(float distance)
{
  std::vector<PrimitiveVertex> out;
  auto size = m_points.size();
  for (unsigned int i = 0; i < size; ++i) {
    BCurve curve = BCurve{ m_points[i],
                           m_ctrl_points[i * 2],
                           m_ctrl_points[i * 2 + 1],
                           m_points[(i + 1) % size] };
    auto vecs = curve.line_along_curve(distance);
    for (auto v : vecs) {
      out.push_back({ { v.x, v.y }, { .5, .5, .5 } });
    }
  }
  return out;
}