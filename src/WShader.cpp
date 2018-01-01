//
// Created by jammer on 16/05/17.
//

#include <WShader.h>
#include <fstream>
#include <iostream>

WShader::WShader(const std::string& vertex_file,
                 const std::string& fragment_file)
{
  GLuint vertex_shader;
  vertex_shader = glCreateShader(GL_VERTEX_SHADER);

  std::ifstream vertex_stream(vertex_file);
  if (!vertex_stream) {
    std::cout << "could not open file: " << vertex_file << std::endl;
    return;
  }
  std::string vertex_shader_src;
  vertex_stream.seekg(0, std::ios::end);
  vertex_shader_src.reserve(vertex_stream.tellg());
  vertex_stream.seekg(0, std::ios::beg);
  vertex_shader_src.assign(std::istreambuf_iterator<char>(vertex_stream),
                           std::istreambuf_iterator<char>());
  auto* vertex_c_str = vertex_shader_src.c_str();

  glShaderSource(vertex_shader, 1, &vertex_c_str, NULL);
  glCompileShader(vertex_shader);

  GLint success;
  GLchar info_log[512];
  glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vertex_shader, 512, NULL, info_log);
    std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
              << info_log << std::endl;
  }

  GLuint fragment_shader;
  fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

  std::ifstream fragment_stream(fragment_file);
  if (!fragment_stream) {
    std::cout << "could not open file: " << fragment_file << std::endl;
    return;
  }
  std::string fragment_shader_src;
  fragment_stream.seekg(0, std::ios::end);
  fragment_shader_src.reserve(fragment_stream.tellg());
  fragment_stream.seekg(0, std::ios::beg);
  fragment_shader_src.assign(std::istreambuf_iterator<char>(fragment_stream),
                             std::istreambuf_iterator<char>());
  auto* fragment_c_str = fragment_shader_src.c_str();

  glShaderSource(fragment_shader, 1, &fragment_c_str, NULL);
  glCompileShader(fragment_shader);

  glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(fragment_shader, 512, NULL, info_log);
    std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
              << info_log << std::endl;
  }

  m_shader = glCreateProgram();
  glAttachShader(m_shader, vertex_shader);
  glAttachShader(m_shader, fragment_shader);
  glLinkProgram(m_shader);

  glGetProgramiv(m_shader, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(m_shader, 512, NULL, info_log);
    std::cout << "ERROR::SHADER::LINK_FAILED\n" << info_log << std::endl;
    return;
  }
  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);
}

void
WShader::use()
{
  glUseProgram(m_shader);
}
