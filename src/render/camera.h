//
// Created by tobi on 4/19/17.
//

#ifndef SINGING_WIND_CAMERA_H
#define SINGING_WIND_CAMERA_H

#include "w_vec.h"

struct Camera
{
  WVec pos;
  float zoom;

  inline bool operator==(const Camera& other)
  {
    return pos == other.pos && zoom == other.zoom;
  }
};

#endif // SINGING_WIND_CAMERA_H
