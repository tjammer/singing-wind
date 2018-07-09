//
// Created by jammer on 16/05/17.
//
#include "renderer.h"
#include "camera.h"
#include <GLFW/glfw3.h>
#include <shader.h>
#include <assert.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>
#include <imgui.h>
#include <iostream>

using namespace WRenderer;

namespace WRenderer {
// vp, models are already transformed
Camera camera;
WVec viewport;
glm::tmat4x4<float> view;
glm::tmat4x4<float> projection;
glm::tmat4x4<float> camera_transform;

int m_mode = -1;
GLuint prim_vao, prim_vbo, prim_ebo;

std::vector<PrimitiveVertex> m_prim_line_verts;
std::vector<PrimitiveVertex> m_prim_quad_verts;
std::vector<unsigned short> m_prim_quad_inds;

std::vector<TexturedVertex> m_tri_quad_verts;
std::vector<unsigned short> m_tri_quad_inds;

WShader primitive_shader;
WShader textured_shader;
}

void ::WRenderer::init(GLFWwindow* window)
{

  glGenBuffers(1, &prim_vbo);
  glGenBuffers(1, &prim_ebo);
  glGenVertexArrays(1, &prim_vao);
  glBindVertexArray(prim_vao);
  glBindBuffer(GL_ARRAY_BUFFER, prim_vbo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, prim_ebo);

  // primitives
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

  primitive_shader = { "shaders/triangle.vs.glsl", "shaders/triangle.fs.glsl" };
  textured_shader = { "shaders/textured.vs.glsl", "shaders/textured.fs.glsl" };

  // camera
  int width, height;
  glfwGetFramebufferSize(window, &width, &height);
  viewport = { width, height };
}

void
WRenderer::add_primitive_vertex(const PrimitiveVertex& vert)
{
  if (m_mode == PLines) {
    m_prim_line_verts.push_back(vert);
  } else if (m_mode == PQuads) {
    m_prim_quad_verts.push_back(vert);
    if (m_prim_quad_verts.size() % 4 == 0) {
      // setup indices
      auto offset = static_cast<unsigned short>(m_prim_quad_verts.size()) - 4;
      for (unsigned short i : { 0, 1, 2, 0, 2, 3 }) {
        m_prim_quad_inds.push_back(offset + i);
      }
    }
  } else {
    assert(false);
  }
}

void
WRenderer::render_array()
{
  GLint transformLoc =
    glGetUniformLocation(primitive_shader.get_program(), "transform");
  glUniformMatrix4fv(
    transformLoc, 1, GL_FALSE, glm::value_ptr(camera_transform));
  primitive_shader.use();
  glBindVertexArray(prim_vao);
  glBindBuffer(GL_ARRAY_BUFFER, prim_vbo);
  glBufferData(GL_ARRAY_BUFFER,
               m_prim_line_verts.size() * sizeof(PrimitiveVertex),
               m_prim_line_verts.data(),
               GL_DYNAMIC_DRAW);

  glDrawArrays(GL_LINES, 0, m_prim_line_verts.size());

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, prim_ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               m_prim_quad_inds.size() * sizeof(unsigned short),
               m_prim_quad_inds.data(),
               GL_DYNAMIC_DRAW);

  glBufferData(GL_ARRAY_BUFFER,
               m_prim_quad_verts.size() * sizeof(PrimitiveVertex),
               m_prim_quad_verts.data(),
               GL_DYNAMIC_DRAW);
  glDrawElements(GL_TRIANGLES, m_prim_quad_inds.size(), GL_UNSIGNED_SHORT, 0);

  glBindVertexArray(0);
}

void
WRenderer::shutdown()
{}

void
WRenderer::reset()
{
  m_prim_line_verts.clear();
  m_prim_quad_verts.clear();
  m_prim_quad_inds.clear();
}

void
WRenderer::set_mode(int mode)
{
  m_mode = mode;
}

void
WRenderer::set_camera(const Camera& a_camera, bool force)
{
  if (camera == a_camera && !force) {
    return;
  }
  camera = a_camera;
  auto half_vp = viewport / 2.f;
  projection = glm::ortho(-half_vp.x * camera.zoom,
                          half_vp.x * camera.zoom,
                          half_vp.y * camera.zoom,
                          -half_vp.y * camera.zoom,
                          .1f,
                          1.f);
  view = glm::lookAt(glm::vec3(camera.pos.x, camera.pos.y, -1),
                     glm::vec3(camera.pos.x, camera.pos.y, 0),
                     glm::vec3(0, -1, 0));
  camera_transform = projection * view;
}

WVec
WRenderer::unproject_mouse(double* pos)
{
  WVec mouse = glm::unProject(
    glm::vec3(pos[0], pos[1], 0),
    glm::translate(glm::tvec3<float>(camera.pos.x, camera.pos.y, 0)),
    projection,
    glm::vec4(0, 0, viewport.x, viewport.y));
  return mouse;
}

void
WRenderer::resize(GLFWwindow* window, int width, int height)
{
  glViewport(0, 0, width, height);
  float fac = viewport.x / (float)width;
  viewport = { width, height };
  set_camera(Camera{ camera.pos, camera.zoom * fac }, true);
}
