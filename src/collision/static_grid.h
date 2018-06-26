#ifndef STATICGRID_H
#define STATICGRID_H

#include "gjk.h"
#include "shapes.h"
#include <vector>
#include <array>
#include <memory>

const int GRID_SIZE = 1000;
const float CELL_WIDTH = 100;

struct StaticPolygon
{
  WVec center;
  float radius;
  PolygonShape shape;
  mutable int tag;
};

struct Cell
{
  size_t first_object{ 0 };
  size_t num_objects{ 0 };
};

class StaticGrid
{
public:
  ColResult test_against_grid(std::unique_ptr<ConvexShape>& a_shape,
                              const Transform& t);

  // RayCastResult raycast_against_grid(const WVec& from, const WVec& to) const
  // {
  //   // starting cell
  //   auto i = get_coord(from.x);
  //   auto j = get_coord(from.y);

  //   // end cell
  //   auto iend = get_coord(to.x);
  //   auto jend = get_coord(to.y);

  //   // which direction to step
  //   size_t di = ((from.x < to.x) ? 1 : ((from.x > to.x) ? -1 : 0));
  //   size_t dj = ((from.y < to.y) ? 1 : ((from.y > to.y) ? -1 : 0));

  //   float minx = m_c_cell_width * floorf(from.x / m_c_cell_width);
  //   float maxx = minx + m_c_cell_width;
  //   float tx = ((from.x < to.x) ? (from.x - minx)
  //                               : (maxx - from.x) / abs(to.x - from.x));
  //   float miny = m_c_cell_width * floorf(from.y / m_c_cell_width);
  //   float maxy = miny + m_c_cell_width;
  //   float ty = ((from.y < to.y) ? (from.y - miny)
  //                               : (maxy - from.y) / abs(to.y - from.y));

  //   float deltax = m_c_cell_width / abs(to.x - from.x);
  //   float deltay = m_c_cell_width / abs(to.y - from.y);

  //   RayCastResult result;
  //   auto dir = glm::normalize(from - to);
  //   auto mag = w_magnitude(from - to);
  //   m_tag++;

  //   while (true) {
  //     // visit cell
  //     auto& cell = m_grid[j][i];
  //     for (size_t ind = 0; ind < cell.num_objects; ++ind) {
  //       auto& object = m_objects[m_indices[cell.first_object + ind]];
  //       if (object.tag != m_tag) {
  //         object.tag = m_tag;
  //         auto cr = cast_ray_vs_shape(from, *object.shape, dir);
  //         if (cr.hit_parameter < result.hit_parameter &&
  //             cr.hit_parameter < mag) {
  //           assert(cr.hits);
  //           result = cr;
  //         }
  //       }
  //     }
  //     if (result.hits) {
  //       break;
  //     }
  //     // continue
  //     if (tx <= ty) {
  //       if (i == iend) {
  //         break;
  //       }
  //       tx += deltax;
  //       i += di;
  //     } else {
  //       if (j == jend) {
  //         break;
  //       }
  //       ty += deltay;
  //       j += dj;
  //     }
  //   }
  //   return result;
  // }

  void lazy_add(StaticPolygon object);

  std::vector<StaticPolygon> find_colliders_in_radius(const WVec& center,
                                                      float radius) const;

  void finish();
  void clear();
  std::vector<StaticPolygon> const& get_objects() const { return m_objects; }

private:
  std::vector<StaticPolygon> m_objects;
  std::vector<size_t> m_indices;
  std::array<std::array<Cell, GRID_SIZE>, GRID_SIZE> m_grid;
  mutable int m_tag{ 0 };
};

#endif /* STATICGRID_H */
