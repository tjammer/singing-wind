#ifndef GAME_H
#define GAME_H

#include "ecs.hpp"
#include "frame_timer.h"
#include "camera.h"
#include "static_grid.h"
// maybe try to fwd-decl

class Game
{
public:
  Game(const WVec& viewport);

  void update();
  void draw();

private:
  ecs::World m_world;
  FrameTimer m_frame_timer;
  Camera m_camera;
  std::unique_ptr<StaticGrid> m_grid;
};

#endif /* GAME_H */
