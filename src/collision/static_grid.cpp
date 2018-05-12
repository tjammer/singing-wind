#include "static_grid.h"
#include "transform.h"
#include "vec_math.h"

inline bool
overlaps(const WVec& center, float radius, const StaticPolygon& b)
{
  return w_magnitude(center - b.center) < radius + b.radius;
}

inline size_t
get_coord(float coord)
{
  return static_cast<int>(floorf((coord) / CELL_WIDTH) + GRID_SIZE / 2.0f);
}

ColResult
StaticGrid::test_against_grid(std::unique_ptr<ConvexShape>& a_shape,
                              const Transform& t)
{
  ColResult result;
  const Transform zerotf;
  m_tag++;

  auto center = transformed(t, a_shape->center());
  size_t x1 = get_coord(center.x - a_shape->radius());
  size_t x2 = get_coord(center.x + a_shape->radius());
  size_t y1 = get_coord(center.y - a_shape->radius());
  size_t y2 = get_coord(center.y + a_shape->radius());
  assert(x1 >= 0 && y1 >= 0 && x2 < GRID_SIZE && y2 < GRID_SIZE);

  for (size_t y = y1; y <= y2; y++) {
    for (size_t x = x1; x <= x2; x++) {
      auto& cell = m_grid[y][x];
      for (size_t i = 0; i < cell.num_objects; ++i) {
        auto& object = m_objects[m_indices[cell.first_object + i]];
        if (object.tag != m_tag) {
          object.tag = m_tag;
          if (!overlaps(
                transformed(t, a_shape->center()), a_shape->radius(), object)) {
            continue;
          }
          ColResult cr = gjk_collide(*a_shape, t, object.shape, zerotf);
          if (cr.collides && cr.depth > result.depth) {
            result = cr;
          }
        }
      }
    }
  }
  return result;
}

void
StaticGrid::lazy_add(StaticPolygon object)
{
  m_objects.push_back(object);

  size_t x1 = get_coord(object.center.x - object.shape.xextends().y);
  size_t x2 = get_coord(object.center.x + object.shape.xextends().x);
  size_t y1 = get_coord(object.center.y - object.shape.yextends().y);
  size_t y2 = get_coord(object.center.y + object.shape.yextends().x);
  assert(x1 >= 0 && y1 >= 0 && x2 < GRID_SIZE && y2 < GRID_SIZE);

  for (size_t y = y1; y <= y2; y++) {
    for (size_t x = x1; x <= x2; x++) {
      auto& cell = m_grid[y][x];
      cell.num_objects++;
    }
  }
}

std::vector<StaticPolygon>
StaticGrid::find_colliders_in_radius(const WVec& center, float radius) const
{
  std::vector<StaticPolygon> colliders;
  m_tag++;
  size_t x1 = get_coord(center.x - radius);
  size_t x2 = get_coord(center.x + radius);
  size_t y1 = get_coord(center.y - radius);
  size_t y2 = get_coord(center.y + radius);
  assert(x1 >= 0 && y1 >= 0 && x2 < GRID_SIZE && y2 < GRID_SIZE);

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
  return colliders;
}
void
StaticGrid::finish()
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
    size_t x1 = get_coord(object.center.x - object.shape.xextends().y);
    size_t x2 = get_coord(object.center.x + object.shape.xextends().x);
    size_t y1 = get_coord(object.center.y - object.shape.yextends().y);
    size_t y2 = get_coord(object.center.y + object.shape.yextends().x);
    assert(x1 >= 0 && y1 >= 0 && x2 < GRID_SIZE && y2 < GRID_SIZE);

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

void
StaticGrid::clear()
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
