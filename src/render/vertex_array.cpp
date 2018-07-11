#include "vertex_array.h"
#include <glad/glad.h>
#include "renderer.h"

template<>
void
VertexArray<PrimitiveVertex>::init(unsigned int a_mode)
{
  mode = a_mode;
  glGenBuffers(1, &vbo);
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);

  glVertexAttribPointer(
    0, 2, GL_FLOAT, GL_FALSE, sizeof(PrimitiveVertex), (GLvoid*)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1,
                        3,
                        GL_FLOAT,
                        GL_FALSE,
                        sizeof(PrimitiveVertex),
                        (GLvoid*)(2 * sizeof(GLfloat)));
  glEnableVertexAttribArray(1);
  glBindVertexArray(0);
}

template<>
void
VertexArray<PrimitiveVertex>::to_gpu() const
{
  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);

  glBufferData(GL_ARRAY_BUFFER,
               vertices.size() * sizeof(PrimitiveVertex),
               vertices.data(),
               GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

template<>
void
VertexArray<PrimitiveVertex>::draw() const
{
  glBindVertexArray(vao);
  glDrawArrays(GL_LINES, 0, vertices.size());
  glBindVertexArray(0);
}

template<>
void
VertexArray<TexturedVertex>::init(unsigned int mode)
{

  glGenBuffers(1, &vbo);
  glGenBuffers(1, &ebo);
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

  // primitives
  glVertexAttribPointer(
    0, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (GLvoid*)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1,
                        2,
                        GL_FLOAT,
                        GL_FALSE,
                        sizeof(TexturedVertex),
                        (GLvoid*)(2 * sizeof(GLfloat)));
  glEnableVertexAttribArray(1);
  glBindVertexArray(0);
}
