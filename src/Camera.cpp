//
// Created by tobi on 4/19/17.
//

#include "Camera.h"
#include <GameWorld.h>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include "PosComponent.h"
#include "WRenderer.h"
#include <glm/gtx/transform.hpp>

void
Camera::update(GameWorld& game_world)
{
  if (game_world.entities().size() > m_entity_to_follow - 1) {
    m_center = game_world.pos_c(m_entity_to_follow).global_position;
  }
  update();
}

WVec
Camera::unproject_mouse(double* pos)
{
  auto viewport = WRenderer::get_viewport();
  WVec mouse = glm::unProject(
    glm::vec3(pos[0], pos[1], 0),
    glm::translate(glm::tvec3<float>(-m_center.x, -m_center.y, 0)),
    m_projection,
    glm::vec4(0, 0, viewport.x, viewport.y));
  return mouse;
}

void
Camera::update(const WVec& offset, float zoom)
{
  m_center += offset;
  m_zoom = zoom;
  update();
}

void
Camera::update()
{
  auto viewport = WRenderer::get_viewport();
  m_projection = glm::ortho(-viewport.x / 2.f * m_zoom,
                            viewport.x / 2.f * m_zoom,
                            -viewport.y / 2.f * m_zoom,
                            viewport.y / 2.f * m_zoom,
                            .1f,
                            1.f);
  m_view = glm::lookAt(glm::vec3(m_center.x, m_center.y, -1),
                       glm::vec3(m_center.x, m_center.y, 0),
                       glm::vec3(0, -1, 0));
  WRenderer::set_camera(m_projection * m_view);
}

void
Camera::set_center(const WVec& center)
{
  m_center = center;
}
