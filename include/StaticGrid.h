#ifndef STATICGRID_H
#define STATICGRID_H

#include "ColShape.h"
#include "CollisionComponent.h"
#include "WVecMath.h"
#include <vector>
#include <array>

const int grid_num_stack = 1000;

struct StaticTriangle
{
  WVec center;
  float radius;
  std::shared_ptr<ColTriangle> shape;
  unsigned int id;
  mutable int tag;
};

struct Cell
{
  size_t first_object{ 0 };
  size_t num_objects{ 0 };
};

template<typename grid_object>
inline bool
overlaps(const WVec& center, float radius, const grid_object& b)
{
  return w_magnitude(center - b.center) < radius + b.radius;
}

template<typename obj>
class StaticGrid
{
public:
  ColResult test_against_grid(std::shared_ptr<ColShape> a_shape)
  {
    ColResult result;
    m_tag++;

    size_t x1 = get_coord(a_shape->m_center.x - a_shape->get_radius());
    size_t x2 = get_coord(a_shape->m_center.x + a_shape->get_radius());
    size_t y1 = get_coord(a_shape->m_center.y - a_shape->get_radius());
    size_t y2 = get_coord(a_shape->m_center.y + a_shape->get_radius());
    assert(x1 >= 0 && y1 >= 0 && x2 < grid_num_stack && y2 < grid_num_stack);

    for (size_t y = y1; y <= y2; y++) {
      for (size_t x = x1; x <= x2; x++) {
        auto& cell = m_grid[y][x];
        for (size_t i = 0; i < cell.num_objects; ++i) {
          auto& object = m_objects[m_indices[cell.first_object + i]];
          if (object.tag != m_tag) {
            object.tag = m_tag;
            if (!overlaps(a_shape->m_center, a_shape->get_radius(), object)) {
              continue;
            }
            auto cr = a_shape->collides(*object.shape);
            if (cr.collides && cr.depth > result.depth) {
              result = cr;
            }
          }
        }
      }
    }
    return result;
  }

  RayCastResult raycast_against_grid(const WVec& from, const WVec& to) const
  {
    // starting cell
    auto i = get_coord(from.x);
    auto j = get_coord(from.y);

    // end cell
    auto iend = get_coord(to.x);
    auto jend = get_coord(to.y);

    // which direction to step
    size_t di = ((from.x < to.x) ? 1 : ((from.x > to.x) ? -1 : 0));
    size_t dj = ((from.y < to.y) ? 1 : ((from.y > to.y) ? -1 : 0));

    float minx = m_c_cell_width * floorf(from.x / m_c_cell_width);
    float maxx = minx + m_c_cell_width;
    float tx = ((from.x < to.x) ? (from.x - minx)
                                : (maxx - from.x) / abs(to.x - from.x));
    float miny = m_c_cell_width * floorf(from.y / m_c_cell_width);
    float maxy = miny + m_c_cell_width;
    float ty = ((from.y < to.y) ? (from.y - miny)
                                : (maxy - from.y) / abs(to.y - from.y));

    float deltax = m_c_cell_width / abs(to.x - from.x);
    float deltay = m_c_cell_width / abs(to.y - from.y);

    RayCastResult result;
    auto dir = glm::normalize(from - to);
    m_tag++;

    while (true) {
      // visit cell
      auto& cell = m_grid[j][i];
      for (size_t ind = 0; ind < cell.num_objects; ++ind) {
        auto& object = m_objects[m_indices[cell.first_object + ind]];
        if (object.tag != m_tag) {
          object.tag = m_tag;
          auto cr = cast_ray_vs_shape(from, *object.shape, dir);
          if (cr.hit_parameter < result.hit_parameter) {
            assert(cr.hits);
            result = cr;
          }
        }
      }
      if (result.hits) {
        break;
      }
      // continue
      if (tx <= ty) {
        if (i == iend) {
          break;
        }
        tx += deltax;
        i += di;
      } else {
        if (j == jend) {
          break;
        }
        ty += deltay;
        j += dj;
      }
    }
    return result;
  }

  void lazy_add(obj object)
  {
    m_objects.push_back(object);

    size_t x1 = get_coord(object.center.x - object.radius);
    size_t x2 = get_coord(object.center.x + object.radius);
    size_t y1 = get_coord(object.center.y - object.radius);
    size_t y2 = get_coord(object.center.y + object.radius);
    assert(x1 >= 0 && y1 >= 0 && x2 < grid_num_stack && y2 < grid_num_stack);

    for (size_t y = y1; y <= y2; y++) {
      for (size_t x = x1; x <= x2; x++) {
        auto& cell = m_grid[y][x];
        cell.num_objects++;
      }
    }
  }

  std::vector<obj> find_colliders_in_radius(const WVec& center,
                                            float radius) const
  {
    std::vector<obj> colliders;
    m_tag++;
    size_t x1 = get_coord(center.x - radius);
    size_t x2 = get_coord(center.x + radius);
    size_t y1 = get_coord(center.y - radius);
    size_t y2 = get_coord(center.y + radius);
    assert(x1 >= 0 && y1 >= 0 && x2 < grid_num_stack && y2 < grid_num_stack);

    for (size_t y = y1; y <= y2; y++) {
      for (size_t x = x1; x <= x2; x++) {
        auto& cell = m_grid[y][x];
        for (size_t ind = 0; ind < cell.num_objects; ++ind) {
          auto& object = m_objects[m_indices[cell.first_object + ind]];
          if (object.tag != m_tag) {
            object.tag = m_tag;
            colliders.push_back(object);
          }
        }
      }
    }
    return std::move(colliders);
  }

  void finish()
  {
    // count and allocate indices array
    size_t ctr{ 0 };
    for (auto& row : m_grid) {
      for (auto& cell : row) {
        cell.first_object = ctr;
        ctr += cell.num_objects;
        cell.num_objects = 0;
      }
    }
    m_indices.resize(ctr);

    for (auto it = m_objects.begin(); it != m_objects.end(); ++it) {
      const auto& object = *it;
      size_t x1 = get_coord(object.center.x - object.radius);
      size_t x2 = get_coord(object.center.x + object.radius);
      size_t y1 = get_coord(object.center.y - object.radius);
      size_t y2 = get_coord(object.center.y + object.radius);
      assert(x1 >= 0 && y1 >= 0 && x2 < grid_num_stack && y2 < grid_num_stack);

      for (size_t y = y1; y <= y2; y++) {
        for (size_t x = x1; x <= x2; x++) {
          auto& cell = m_grid[y][x];
          m_indices[cell.first_object + cell.num_objects] =
            it - m_objects.begin();
          cell.num_objects++;
        }
      }
    }
  }

  void clear()
  {
    m_objects.clear();
    m_indices.clear();
    for (auto& row : m_grid) {
      for (auto& cell : row) {
        cell.num_objects = 0;
        cell.first_object = 0;
      }
    }
  }
  std::vector<obj> const& get_objects() const { return m_objects; }

private:
  std::vector<obj> m_objects;
  std::vector<size_t> m_indices;
  std::array<std::array<Cell, grid_num_stack>, grid_num_stack> m_grid;
  float m_c_cell_width = c_line_triangulate_split * 1.5f;
  mutable int m_tag{ 0 };

  inline size_t get_coord(float coord) const
  {
    return static_cast<int>(floorf((coord) / m_c_cell_width) +
                            grid_num_stack / 2.0f);
  }
};

#endif /* STATICGRID_H */
