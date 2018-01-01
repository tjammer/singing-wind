//
// Created by tobi on 3/27/17.
//

#ifndef SINGING_WIND_STATEBASE_H
#define SINGING_WIND_STATEBASE_H

#include <vector>
#include <memory>
#include <limits>
#include <WindDefs.h>

class EngineState;
class GameWorld;
struct GLFWwindow;

// engine base class, different from gameplay states
class Engine
{
public:
  void init();

  void change_state(EngineState& state);
  void push_state(EngineState& state);
  void pop_state();

  void update();
  void draw();

  Engine(GLFWwindow& window);
  ~Engine();

  GLFWwindow& get_window() const { return window; }

  void set_focus(bool focus) { m_has_focus = focus; }
  bool get_focus() const { return m_has_focus; }

  GameWorld& get_world();

  // mouse wheel
  float get_mouse_wheel() { return m_mouse_wheel; }
  void update_mouse_wheel(float delta) { m_mouse_wheel += delta; }

private:
  bool m_has_focus = true;
  std::vector<std::unique_ptr<EngineState>> m_states;
  GLFWwindow& window;
  size_t m_game_index = std::numeric_limits<long>::max();
  size_t m_editor_index = std::numeric_limits<long>::max();

  // switch
  bool m_switch_pressed = false;
  float m_mouse_wheel = 0;
};

class EngineState
{
public:
  virtual void cleanup() = 0;

  virtual void pause() { m_paused = true; }
  virtual void unpause() { m_paused = false; }
  virtual void switch_pause()
  {
    if (m_paused) {
      unpause();
      return;
    }
    pause();
  }

  virtual void update(Engine& engine) = 0;
  virtual void draw() = 0;

  virtual ~EngineState() = default;

  bool m_paused = false;
};

#endif // SINGING_WIND_STATEBASE_H
