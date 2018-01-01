//
// Created by jammer on 08/04/17.
//

#ifndef SINGING_WIND_GAME_H
#define SINGING_WIND_GAME_H

#include "Engine.h"
#include "GameWorld.h"
#include "Camera.h"
#include "FrameTimer.h"

// the game state of the engine FSM
class Game : public EngineState
{
public:
  void cleanup() override{};
  void unpause() override;

  void update(Engine& engine) override;
  void draw() override;

  Game();
  ~Game() = default;

  bool m_paused = false;

  GameWorld& get_world() { return m_game_world; };

private:
  GameWorld m_game_world;
  Camera m_camera;
  FrameTimer m_timer;

  void update_camera_follow();
};

#endif // SINGING_WIND_GAME_H
