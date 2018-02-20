#ifndef WRAPAROUND_H
#define WRAPAROUND_H

const float wrapsize = 2 * 1500;

template<typename T>
T
wrap_add(const T& a, const T& b)
{
  return a + b;
}

template<typename T>
T
wrap_sub(const T& a, const T& b)
{
  return a - b;
}

#endif /* WRAPAROUND_H */
