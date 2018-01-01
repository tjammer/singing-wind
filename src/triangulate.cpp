//
// Created by jammer on 03/04/17.
//

#include "triangulate.h"
#include "BCurve.h"
#include "ColShape.h"

void
triangulate_island(const Island& island, std::vector<WVec>& tri_vector)
{
  // build vertex vector
  std::vector<WVec> verts;
  for (unsigned int i = 0; i < island.m_points.size(); ++i) {
    const auto curve =
      BCurve{ island.m_points[i],
              island.m_ctrl_points[i * 2],
              island.m_ctrl_points[i * 2 + 1],
              island.m_points[(i + 1) % island.m_points.size()] };
    auto points = curve.points_along_curve(c_line_triangulate_split);
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
