#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H

#include <array>

template<typename T, unsigned int size>
class input_buffer
{
public:
  input_buffer() {}

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
  const T& get() const { return m_buf[m_index]; }
  void fill(const T& val) { m_buf.fill(val); }

private:
  std::array<T, size> m_buf;
  size_t m_index = 0;
};

#endif /* CIRCULAR_BUFFER_H */
