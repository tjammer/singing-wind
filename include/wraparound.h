#ifndef WRAPAROUND_H
#define WRAPAROUND_H

const float wrapsize = 2 * 1500;

template<typename T>
T
get_wrapped_global_position(const T& query, const T& other)
{
  auto diff = query.x - other.x;
  auto x = query.x;
  if (abs(diff) > abs(diff + wrapsize)) {
    x += wrapsize;
  } else if (abs(diff) > abs(diff - wrapsize)) {
    x -= wrapsize;
  }
  return { x, query.y };
}

#endif /* WRAPAROUND_H */
