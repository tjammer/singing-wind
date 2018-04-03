//
// Created by jammer on 16/05/17.
//

#ifndef GLFW33_WSHADER_H
#define GLFW33_WSHADER_H

#include <glad/glad.h>
#include <string>

class WShader
{
public:
  WShader(const std::string& vertex_file, const std::string& fragment_file);
  WShader() = default;
  ~WShader() = default;

  void use();
  GLuint get_program() const { return m_shader; }

private:
  GLuint m_shader;
};

#endif // GLFW33_WSHADER_H
