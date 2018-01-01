//
// Created by tobi on 3/27/17.
//

#include "Engine.h"
#include "Editor.h"
#include "Game.h"
#include "WRenderer.h"

Engine::Engine(GLFWwindow& window)
  : window(window)
{
  WRenderer::init(&window);

  m_states.emplace_back(new Game);
  m_game_index = m_states.size() - 1;
  m_states.emplace_back(new EngineEditorState("debug", get_world()));
  m_editor_index = m_states.size() - 1;
  // transition to editor
  m_states[m_game_index]->pause();
}

void
Engine::update()
{
  if (glfwGetKey(&window, GLFW_KEY_F1) == GLFW_PRESS and not m_switch_pressed) {
    m_states[m_game_index]->switch_pause();
    m_states[m_editor_index]->switch_pause();
  }
  m_switch_pressed = glfwGetKey(&window, GLFW_KEY_F1) == GLFW_PRESS;

  WRenderer::reset();

  for (auto& state : m_states) {
    if (!state->m_paused) {
      state->update(*this);
    }
  }
  draw();
}

void
Engine::draw()
{
  glClearColor(0.16f, 0.19f, 0.23f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
  for (auto& state : m_states) {
    state->draw();
  }
  WRenderer::render_array();
}

GameWorld&
Engine::get_world()
{
  return (*dynamic_cast<Game*>(m_states[m_game_index].get())).get_world();
}

Engine::~Engine()
{
  WRenderer::shutdown();
}
