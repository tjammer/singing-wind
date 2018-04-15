//
// Created by tobi on 4/19/17.
//

#include "camera.h"
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>

Camera::Camera(const WVec& viewport)
  : m_viewport(viewport)
{

  float fac = 1.0 / 2.0;
  m_projection = glm::ortho(-viewport.x * fac,
                            viewport.x * fac,
                            viewport.y * fac,
                            -viewport.y * fac,
                            .1f,
                            1.f);
  m_view =
    glm::lookAt(glm::vec3(0, 0, -1), glm::vec3(0, 0, 0), glm::vec3(0, -1, 0));
}

WVec
Camera::unproject_mouse(double* pos) const
{
  WVec mouse = glm::unProject(glm::vec3(pos[0], pos[1], 0),
                              glm::translate(glm::tvec3<float>(0, 0, 0)),
                              m_projection,
                              glm::vec4(0, 0, m_viewport.x, m_viewport.y));
  return mouse;
}

glm::tmat4x4<float>
Camera::get_camera() const
{
  return m_projection * m_view;
}
