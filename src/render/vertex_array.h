#ifndef VERTEX_ARRAY_H
#define VERTEX_ARRAY_H

#include <vector>
#include <glad/glad.h>

template<typename T>
struct VertexArray
{
  unsigned int vao;
  unsigned int vbo;
  unsigned int ebo;
  std::vector<T> vertices;
  std::vector<unsigned int> indices;
  unsigned int mode;

  void init(unsigned int mode);
  void to_gpu() const
  {
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(GL_ARRAY_BUFFER,
                 vertices.size() * sizeof(T),
                 vertices.data(),
                 GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 indices.size() * sizeof(unsigned int),
                 indices.data(),
                 GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
  }

  void draw() const
  {
    glBindVertexArray(vao);
    glDrawElements(mode, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
  }

  void clear()
  {
    vertices.clear();
    indices.clear();
  }
};

#endif /* VERTEX_ARRAY_H */
