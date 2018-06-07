//
// Created by tobi on 4/19/17.
//

#include "camera.h"
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>

class Camera::impl
{
public:
  glm::tmat4x4<float> projection;
  glm::tmat4x4<float> view;
  WVec viewport;
  impl(const WVec& viewport)
    : viewport(viewport)
  {}
};

Camera::~Camera() = default;

Camera::Camera(const WVec& viewport)
  : m_pimpl(std::make_unique<impl>(viewport))
{
  init();
}

void
Camera::init()
{
  auto& viewport = m_pimpl->viewport;
  float fac = 1.0 / 2.0;
  m_pimpl->projection = glm::ortho(-viewport.x * fac,
                                   viewport.x * fac,
                                   viewport.y * fac,
                                   -viewport.y * fac,
                                   .1f,
                                   1.f);
  m_pimpl->view =
    glm::lookAt(glm::vec3(0, 0, -1), glm::vec3(0, 0, 0), glm::vec3(0, -1, 0));
}

WVec
Camera::unproject_mouse(double* pos) const
{
  WVec mouse =
    glm::unProject(glm::vec3(pos[0], pos[1], 0),
                   glm::translate(glm::tvec3<float>(0, 0, 0)),
                   m_pimpl->projection,
                   glm::vec4(0, 0, m_pimpl->viewport.x, m_pimpl->viewport.y));
  return mouse;
}

glm::tmat4x4<float>
Camera::get_camera() const
{
  return m_pimpl->projection * m_pimpl->view;
}

void
Camera::reset(const WVec& viewport)
{
  m_pimpl = std::make_unique<impl>(viewport);
  init();
}
