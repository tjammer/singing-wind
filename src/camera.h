//
// Created by tobi on 4/19/17.
//

#ifndef SINGING_WIND_CAMERA_H
#define SINGING_WIND_CAMERA_H

#include "wind_defs.h"

class Camera
{
public:
  unsigned int m_entity_to_follow;
  void update(const WVec& offset, float zoom);
  void set_center(const WVec& center);
  WVec unproject_mouse(double pos[2]);

private:
  glm::tmat4x4<float> m_projection;
  glm::tmat4x4<float> m_view;
  float m_zoom = 1.7f;
  WVec m_center;
  void update();
};

#endif // SINGING_WIND_CAMERA_H
