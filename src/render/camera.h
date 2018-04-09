//
// Created by tobi on 4/19/17.
//

#ifndef SINGING_WIND_CAMERA_H
#define SINGING_WIND_CAMERA_H

#include "wind_defs.h"

class Camera
{
public:
  WVec unproject_mouse(double pos[2]) const;
  glm::tmat4x4<float> get_camera() const;
  Camera(const WVec& viewport);

private:
  glm::tmat4x4<float> m_projection;
  glm::tmat4x4<float> m_view;
  WVec m_viewport;
};

#endif // SINGING_WIND_CAMERA_H
