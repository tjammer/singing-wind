//
// Created by jammer on 16/05/17.
//

#ifndef GLFW33_WIMAGE_H
#define GLFW33_WIMAGE_H

#include <string>
#include <glad/glad.h>

class WTexture
{
public:
  WTexture(const std::string& filename);
  ~WTexture();

  GLuint get() const { return m_tex; }
  int width() const { return m_x; }
  int height() const { return m_y; }

private:
  int m_x;
  int m_y;
  GLuint m_tex;
};

#endif // GLFW33_WIMAGE_H
