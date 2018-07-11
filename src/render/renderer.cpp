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

WShader primitive_shader;
WShader textured_shader;
}

void
WRenderer::init(GLFWwindow* window)
{
  m_prim_va.init(GL_LINES);
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
  if (m_mode != PLines) {
    assert(false);
  }
  m_prim_va.vertices.push_back(vert);
}

void
WRenderer::render_array()
{
  GLint transformLoc =
    glGetUniformLocation(primitive_shader.get_program(), "transform");
  glUniformMatrix4fv(
    transformLoc, 1, GL_FALSE, glm::value_ptr(camera_transform));
  primitive_shader.use();

  m_prim_va.to_gpu();
  m_prim_va.draw();
}

void
WRenderer::shutdown()
{}

void
WRenderer::reset()
{
  m_prim_va.vertices.clear();
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
