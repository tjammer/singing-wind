//
// Created by jammer on 16/05/17.
//
#include "renderer.h"
#include "camera.h"
#include "vertex_array.h"
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

VertexArray<PrimitiveVertex> m_prim_va;
VertexArray<TexturedVertex> m_tex_va;

WShader primitive_shader;
WShader textured_shader;

unsigned int active_texture = 0;
}

void
WRenderer::init(GLFWwindow* window)
{
  m_prim_va.init(GL_LINES);
  m_tex_va.init(GL_TRIANGLES);
  primitive_shader = { "shaders/triangle.vs.glsl", "shaders/triangle.fs.glsl" };
  textured_shader = { "shaders/textured.vs.glsl", "shaders/textured.fs.glsl" };

  // camera
  int width, height;
  glfwGetFramebufferSize(window, &width, &height);
  viewport = { width, height };

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void
WRenderer::add_primitive_vertex(const PrimitiveVertex& vert)
{
  if (m_mode != PLines) {
    assert(false);
  }
  m_prim_va.vertices.push_back(vert);
}

void
WRenderer::add_sprite(const WVec& center, const WVec& size, unsigned int tex)
{
  active_texture = tex;
  m_tex_va.vertices.push_back(
    { { center.x - size.x, center.y - size.y }, { 0.0f, 0.0f } });
  m_tex_va.vertices.push_back(
    { { center.x - size.x, center.y + size.y }, { 0.0f, 1.0f } });
  m_tex_va.vertices.push_back(
    { { center.x + size.x, center.y + size.y }, { 1.0f, 1.0f } });
  m_tex_va.vertices.push_back(
    { { center.x + size.x, center.y - size.y }, { 1.0f, 0.0f } });

  m_tex_va.indices.push_back(0);
  m_tex_va.indices.push_back(1);
  m_tex_va.indices.push_back(2);
  m_tex_va.indices.push_back(0);
  m_tex_va.indices.push_back(2);
  m_tex_va.indices.push_back(3);
}

void
WRenderer::render_array()
{
  m_prim_va.to_gpu();
  m_tex_va.to_gpu();

  primitive_shader.use();
  glUniformMatrix4fv(
    glGetUniformLocation(primitive_shader.get_program(), "transform"),
    1,
    GL_FALSE,
    glm::value_ptr(camera_transform));
  m_prim_va.draw();

  glBindTexture(GL_TEXTURE_2D, active_texture);
  textured_shader.use();
  glUniformMatrix4fv(
    glGetUniformLocation(textured_shader.get_program(), "transform"),
    1,
    GL_FALSE,
    glm::value_ptr(camera_transform));
  m_tex_va.draw();
}

void
WRenderer::shutdown()
{}

void
WRenderer::reset()
{
  m_prim_va.clear();
  m_tex_va.clear();
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
