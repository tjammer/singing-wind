//
// Created by jammer on 03/04/17.
//

#include "triangulate.h"
#include "BCurve.h"
#include <limits>

#include <iostream>

void
triangulate_island(const Island& island,
                   std::vector<WVec>& tri_vector,
                   float split_length)
{
  // build vertex vector
  std::vector<WVec> verts;
  for (unsigned int i = 0; i < island.m_points.size(); ++i) {
    const auto curve =
      BCurve{ island.m_points[i],
              island.m_ctrl_points[i * 2],
              island.m_ctrl_points[i * 2 + 1],
              island.m_points[(i + 1) % island.m_points.size()] };
    auto points = curve.points_along_curve(split_length);
    for (const auto& point : points) {
      verts.push_back(point);
    }
  }

  auto size = verts.size();

  std::vector<float> points(size * 2);
  std::vector<int> segments(size * 2);
  for (unsigned int i = 0; i < size; ++i) {
    points[i * 2] = verts[i].x;
    points[i * 2 + 1] = verts[i].y;
    segments[i * 2] = i;
    segments[i * 2 + 1] = static_cast<int>((i + 1) % size);
  }
  triangulateio in;
  in.pointlist = points.data();
  in.numberofpoints = static_cast<int>(size);
  in.numberofpointattributes = 0;
  in.pointmarkerlist = nullptr;
  in.segmentlist = segments.data();
  in.segmentmarkerlist = nullptr;
  in.numberofsegments = in.numberofpoints;
  in.numberofholes = 0;
  in.numberofregions = 0;

  std::vector<float> out_points(size * 4);
  std::vector<int> out_segments(size * 4);
  std::vector<int> triangles(size * 10);
  triangulateio out;
  out.pointlist = out_points.data();
  out.pointmarkerlist = nullptr;
  out.trianglelist = triangles.data();
  out.segmentlist = out_segments.data();
  out.triangleattributelist = nullptr;
  out.segmentmarkerlist = nullptr;

  char flags[5] = { 'p', 'q', 'z', 'Q', '\0' };
  triangulate(flags, &in, &out, nullptr);
  triangles.resize(static_cast<size_t>(out.numberoftriangles * 3));
  tri_vector.clear();
  for (auto index : triangles) {
    tri_vector.push_back(
      WVec(out_points[index * 2], out_points[index * 2 + 1]));
  }
}

void
voronoi_negative_space(const std::vector<Island>& a_islands,
                       std::vector<WVec>& a_points)
{
  int num_verts = 0;
  // get farthest points in each direction
  const float mx = std::numeric_limits<float>::max();
  const float mn = std::numeric_limits<float>::min();
  float x_mx = mn;
  float y_mx = mn;
  float x_mn = mx;
  float y_mn = mx;

  for (const auto& island : a_islands) {
    for (const auto& p : island.m_points) {
      num_verts++;
      if (p.x > x_mx) {
        x_mx = p.x;
      } else if (p.x < x_mn) {
        x_mn = p.x;
      }
      if (p.y > y_mx) {
        y_mx = p.y;
      } else if (p.y < y_mn) {
        y_mn = p.y;
      }
    }
  }

  // add triangle to io struct
  triangulateio in;
  std::vector<WVec> verts;
  // verts.emplace_back(x_mx, y_mx);
  // verts.emplace_back(x_mx, y_mn);
  // verts.emplace_back(x_mn, y_mn);
  // verts.emplace_back(x_mn, y_mx);
  //
  verts.emplace_back(10.1f, 10.f);
  verts.emplace_back(0.f, 10.1f);
  verts.emplace_back(0.f, 0.1f);
  verts.emplace_back(10.f, 0.f);

  auto size = verts.size();
  std::vector<float> points(size * 2);
  std::vector<int> segments(size * 2);
  for (unsigned int i = 0; i < size; ++i) {
    points[i * 2] = verts[i].x;
    points[i * 2 + 1] = verts[i].y;
    segments[i * 2] = i;
    segments[i * 2 + 1] = static_cast<int>((i + 1) % size);
  }

  for (auto vec : points) {
    std::cout << vec << std::endl;
  }
  assert(size == 4);

  in.pointlist = points.data();
  in.numberofpoints = static_cast<int>(points.size());
  in.numberofpointattributes = 0;
  in.pointmarkerlist = nullptr;
  in.segmentlist = segments.data();
  in.segmentmarkerlist = nullptr;
  in.numberofsegments = in.numberofpoints;
  in.numberofholes = 0;
  in.numberofregions = 0;

  // add triangle vertices as holes
  std::vector<float> out_points(size * 4);
  std::vector<int> out_segments(size * 4);
  std::vector<int> triangles(size * 10);
  triangulateio out;
  out.pointlist = out_points.data();
  out.pointmarkerlist = nullptr;
  out.trianglelist = triangles.data();
  out.segmentlist = out_segments.data();
  out.triangleattributelist = nullptr;
  out.segmentmarkerlist = nullptr;

  char flags[5] = { 'p', 'z', 'q', 'Q', '\0' };
  triangulate(flags, &in, &out, nullptr);
  triangles.resize(static_cast<size_t>(out.numberoftriangles * 3));
  std::cout << out.numberoftriangles << std::endl;
  a_points.clear();
  for (auto index : triangles) {
    a_points.push_back(WVec(out_points[index * 2], out_points[index * 2 + 1]));
  }

  // make voronoi
}
