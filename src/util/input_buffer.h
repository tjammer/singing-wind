#ifndef INPUT_BUFFER_H
#define INPUT_BUFFER_H

#include <array>

template<typename T, unsigned int size>
class CircularBuffer
{
public:
  CircularBuffer() {}

  void push(const T& val)
  {
    m_index = (m_index + 1) % size;
    m_buf[m_index] = val;
  }

  bool just_added(const T& val) const
  {
    if (m_buf[m_index] != val) {
      return false;
    }
    for (unsigned int i = 1; i < size; ++i) {
      if (m_buf[(m_index + i) % size] != val) {
        return true;
      }
    }
    return false;
  }

  bool is(const T& val) const { return m_buf[m_index] == val; }
  const T& current() const { return m_buf[m_index]; }
  void fill(const T& val) { m_buf.fill(val); }

private:
  std::array<T, size> m_buf;
  size_t m_index = 0;
};

const int INPUT_BUFFER_LEN = 10;

template<typename T>
using InputBuffer = CircularBuffer<T, INPUT_BUFFER_LEN>;

#endif /* INPUT_BUFFER_H */
