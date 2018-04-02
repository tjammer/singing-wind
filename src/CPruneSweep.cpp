#include "CPruneSweep.h"
#include "wraparound.h"
#include <algorithm>

struct cmp_func
{
  size_t sort_axis;
  bool operator()(const PSBox& a, const PSBox& b)
  {
    return a.mins[sort_axis] <= b.mins[sort_axis];
  }
};

inline bool
overlap(const PSBox& a, const PSBox& b, float offset = 0)
{
  for (size_t i = 0; i < 2; ++i) {
    if (a.maxs[i] < b.mins[i] + offset)
      return false;
    if (a.mins[i] > b.maxs[i] - offset)
      return false;
  }
  return true;
}

void
PruneSweeper::prune_and_sweep()
{
  std::sort(m_boxes.begin(), m_boxes.end(), cmp_func{ sort_axis });
  m_pairs.clear();

  glm::tvec2<float> means{ 0, 0 };
  glm::tvec2<float> sqrd_means{ 0, 0 };

  for (unsigned int i = 0; i < m_boxes.size(); ++i) {
    const auto& box = m_boxes[i];
    glm::tvec2<float> p_mn = 0.5f * (box.mins + box.maxs);

    means += p_mn;
    sqrd_means += p_mn * p_mn;

    for (unsigned int j = i + 1; j < m_boxes.size(); ++j) {
      if (m_boxes[j].mins[sort_axis] > box.maxs[sort_axis]) {
        break;
      }
      if (overlap(box, m_boxes[j])) {
        m_pairs.emplace_back(box.entity, m_boxes[j].entity);
      }
    }
    assert(sort_axis == 0);

    // check upper wrap
    for (unsigned int j = 0; j < i; ++j) {
      if (m_boxes[j].mins[sort_axis] + wrapsize > box.maxs[sort_axis]) {
        break;
      }
      if (overlap(box, m_boxes[j]), +wrapsize) {
        m_pairs.emplace_back(box.entity, m_boxes[j].entity);
      }
    }
    // check lower wrap
    for (unsigned int j = m_boxes.size() - 1; j > i; --j) {
      if (m_boxes[j].maxs[sort_axis] - wrapsize < box.mins[sort_axis]) {
        break;
      }
      if (overlap(box, m_boxes[j]), -wrapsize) {
        m_pairs.emplace_back(box.entity, m_boxes[j].entity);
      }
    }
  }

  // glm::tvec2<float> var = sqrd_means - means * means / (float)m_boxes.size();
  // sort_axis = 0;
  // if (var.y > var.x) {
  //  sort_axis = 1;
  //}
}

std::vector<PSBox>
PruneSweeper::find_in_radius(const WVec& center,
                             float radius,
                             unsigned int entity)
{
  PSBox box{ center - radius, center + radius, entity };
  auto lower = std::lower_bound(
    m_boxes.begin(), m_boxes.end(), box, cmp_func{ sort_axis });
  std::vector<PSBox> boxes;

  // check upper dir
  for (size_t i = lower - m_boxes.begin(); i < m_boxes.size(); ++i) {
    if (m_boxes[i].mins[sort_axis] > box.maxs[sort_axis]) {
      break;
    }
    if (overlap(box, m_boxes[i]) && m_boxes[i].entity != entity) {
      boxes.push_back(m_boxes[i]);
    }
  }
  // check lower dir
  for (auto it = lower; it > m_boxes.begin(); --it) {
    auto& other = *it;
    if (other.maxs[sort_axis] < box.mins[sort_axis]) {
      break;
    }
    if (overlap(box, other) && other.entity != entity) {
      boxes.push_back(other);
    }
  }
  // check for wraps
  // check upper wrap
  for (auto j = 0; j < lower - m_boxes.begin(); ++j) {
    if (m_boxes[j].mins[sort_axis] + wrapsize > box.maxs[sort_axis]) {
      break;
    }
    if (overlap(box, m_boxes[j]), +wrapsize) {
      boxes.push_back(m_boxes[j]);
    }
  }
  // check lower wrap
  for (unsigned int j = m_boxes.size() - 1; j > lower - m_boxes.begin(); --j) {
    if (m_boxes[j].maxs[sort_axis] - wrapsize < box.mins[sort_axis]) {
      break;
    }
    if (overlap(box, m_boxes[j]), -wrapsize) {
      boxes.push_back(m_boxes[j]);
    }
  }
  return boxes;
}

void
PruneSweeper::clear()
{
  m_boxes.clear();
  m_pairs.clear();
}
