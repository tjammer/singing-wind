//
// Created by tobi on 4/19/17.
//

#ifndef SINGING_WIND_CAMERA_H
#define SINGING_WIND_CAMERA_H

#include "w_vec.h"
#include "glm/fwd.hpp"
#include <memory>

class Camera
{
public:
  WVec unproject_mouse(double pos[2]) const;
  glm::tmat4x4<float, glm::precision::defaultp> get_camera() const;
  Camera(const WVec& viewport);
  ~Camera();

private:
  class impl;
  std::unique_ptr<impl> m_pimpl;
};

#endif // SINGING_WIND_CAMERA_H
