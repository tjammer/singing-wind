#include "game.h"
#include "comps.h"
#include "input.h"
#include "input_system.h"
#include "flying.h"
#include "renderer.h"
#include "move.h"
#include "draw.h"
#include "nlohmann/json.hpp"

Game::Game(const WVec& viewport)
  : m_world(ecs::World{})
  , m_frame_timer(FrameTimer{})
  , m_camera(Camera{ viewport })
{
  auto player = m_world.create_entity();
  m_world.create_component<Input>(player, Input{});
  m_world.create_component<Position>(player, Position{});
  auto& pc = m_world.get_component<Position>(player);
  pc.position.y = 300;
  m_world.create_component(player, Movement{});

  WRenderer::set_camera(m_camera.get_camera());
}

void
Game::update()
{
  WInput::set_mouse(m_camera.unproject_mouse(WInput::get_raw_mouse()));

  m_frame_timer.update();
  while (m_frame_timer.pop_fixed()) {
    m_world.visit(input_update);
    // accel systems
    m_world.visit(dummy_flying);
    // now integrate
    m_world.visit(
      [](Movement& mc, Position& pc) { move_update(mc, pc, FIXED_TIMESTEP); });
  }
  m_world.visit(draw_update);
}

void
Game::draw()
{
  WRenderer::render_array();
}
