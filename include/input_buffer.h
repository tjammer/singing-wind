#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H

#include <array>

template <typename T, unsigned int size>
class input_buffer {
public:
    input_buffer() : m_size(size) {}

    void push(T val) {
        m_index = (m_index + 1) % size;
        m_buf[m_index] = val;
    }
    bool just_added(T val) const {
        if (m_buf[m_index] != val) {
            return false;
        }
        for (unsigned int i = 1 ; i < m_size ; ++i) {
            if (m_buf[(m_index + i) % m_size] != val) {
                return true;
            }
        }
        return false;
    }
    T get() const {
        return m_buf[m_index];
    }
    void fill(T val) {
        m_buf.fill(val);
    }

private:
    std::array<T, size> m_buf;
    size_t m_index = 0;
    size_t m_size;
};

#endif /* CIRCULAR_BUFFER_H */
