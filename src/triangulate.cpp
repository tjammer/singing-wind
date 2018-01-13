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

  char flags[] = { 'p', 'z', 'Q', '\0' };
  triangulate(flags, &in, &out, nullptr);
  triangles.resize(static_cast<size_t>(out.numberoftriangles * 3));
  tri_vector.clear();
  for (auto index : triangles) {
    tri_vector.push_back(
      WVec(out_points[index * 2], out_points[index * 2 + 1]));
  }
}

void
triangulate_between_islands(const std::vector<Island>& a_islands,
                            std::vector<QuickTri>& a_tris,
                            float split_length)
{
  // add triangle to io struct
  std::vector<float> verts;
  std::vector<int> segments;
  std::vector<float> holes;

  float x_lm[2] = { std::numeric_limits<float>::max(),
                    std::numeric_limits<float>::min() };
  float y_lm[2] = { std::numeric_limits<float>::max(),
                    std::numeric_limits<float>::min() };

  int vec_count = 0;
  for (const auto& island : a_islands) {
    int island_vec_ct = 0;
    for (unsigned int i = 0; i < island.m_points.size(); ++i) {
      const auto curve =
        BCurve{ island.m_points[i],
                island.m_ctrl_points[i * 2],
                island.m_ctrl_points[i * 2 + 1],
                island.m_points[(i + 1) % island.m_points.size()] };
      auto points = curve.points_along_curve(split_length);
      for (auto it = points.begin(); it != points.end(); ++it) {
        auto& point = *it;
        verts.push_back(point.x);
        verts.push_back(point.y);
        segments.push_back(vec_count);
        segments.push_back(static_cast<int>(vec_count + 1));
        island_vec_ct++;
        vec_count++;

        if (point.x < x_lm[0]) {
          x_lm[0] = point.x;
        } else if (point.x > x_lm[1]) {
          x_lm[1] = point.x;
        }
        if (point.y < y_lm[0]) {
          y_lm[0] = point.y;
        } else if (point.y > y_lm[1]) {
          y_lm[1] = point.y;
        }
      }
    }
    segments[segments.size() - 1] = vec_count - island_vec_ct;
    std::vector<WVec> tris;
    triangulate_island(island, tris, split_length);
    auto center = (tris[0] + tris[1] + tris[2]) / 3.0f;
    holes.push_back(center.x);
    holes.push_back(center.y);
  }

  verts.push_back(x_lm[0] - split_length);
  verts.push_back(y_lm[0] - split_length);

  verts.push_back(x_lm[0] - split_length);
  verts.push_back(y_lm[1] + split_length);

  verts.push_back(x_lm[1] + split_length);
  verts.push_back(y_lm[1] + split_length);

  verts.push_back(x_lm[1] + split_length);
  verts.push_back(y_lm[0] - split_length);

  auto size = verts.size() / 2;

  segments.push_back(size - 4);
  segments.push_back(size - 3);

  segments.push_back(size - 3);
  segments.push_back(size - 2);

  segments.push_back(size - 2);
  segments.push_back(size - 1);

  segments.push_back(size - 1);
  segments.push_back(size - 4);

  triangulateio in;
  in.pointlist = verts.data();
  in.numberofpoints = static_cast<int>(verts.size() / 2);
  in.numberofpointattributes = 0;
  in.pointmarkerlist = nullptr;
  in.segmentlist = segments.data();
  in.segmentmarkerlist = nullptr;
  in.numberofsegments = static_cast<int>(segments.size() / 2);
  in.numberofholes = static_cast<int>(holes.size() / 2);
  in.holelist = holes.data();
  in.numberofregions = 0;

  std::vector<float> out_points(size * 40);
  std::vector<int> out_segments(size * 40);
  std::vector<int> triangles(size * 100);
  triangulateio out;
  out.pointlist = out_points.data();
  out.pointmarkerlist = nullptr;
  out.trianglelist = triangles.data();
  out.segmentlist = out_segments.data();
  out.triangleattributelist = nullptr;
  out.segmentmarkerlist = nullptr;

  std::vector<float> vor_points(size * 100);
  std::vector<float> vor_norms(size * 100);
  std::vector<int> vor_edges(size * 40);
  triangulateio vor;
  vor.pointlist = vor_points.data();
  vor.edgelist = vor_edges.data();
  vor.normlist = vor_norms.data();

  char flags[] = { 'z', 'p', 'q', 'Q', '\0' };
  triangulate(flags, &in, &out, &vor);
  triangles.resize(static_cast<size_t>(out.numberoftriangles * 3));
  assert(out.numberoftriangles > 0);
  a_tris.clear();
  for (int i = 0; i < out.numberoftriangles; ++i) {
    auto i1 = triangles[i * 3];
    auto i2 = triangles[i * 3 + 1];
    auto i3 = triangles[i * 3 + 2];

    auto p1 = WVec{ out_points[i1 * 2], out_points[i1 * 2 + 1] };
    auto p2 = WVec{ out_points[i2 * 2], out_points[i2 * 2 + 1] };
    auto p3 = WVec{ out_points[i3 * 2], out_points[i3 * 2 + 1] };
    a_tris.push_back(QuickTri{ p1, p2, p3 });
  }
}
