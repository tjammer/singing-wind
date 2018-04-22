#include "game.h"
#include "comps.h"
#include "input.h"
#include "input_system.h"
#include "flying.h"
#include "renderer.h"
#include "move.h"
#include "draw.h"
#include "flatbuffers/flatbuffers.h"
#include "map_generated.h"
#include <fstream>
#include <iostream>

Game::Game(const WVec& viewport)
  : m_world(ecs::World{})
  , m_frame_timer(FrameTimer{})
  , m_camera(Camera{ viewport })
  , m_grid(std::make_unique<StaticGrid<StaticTriangle>>())
{
  auto player = m_world.create_entity();
  m_world.create_component(player, Input{});
  m_world.create_component(player, Position{});
  m_world.create_component(
    player, Collision{ {}, std::make_shared<ColCapsule>(10, 20) });
  auto& pc = m_world.get_component<Position>(player);
  pc.position.y = 300;
  m_world.create_component(player, Movement{});

  WRenderer::set_camera(m_camera.get_camera());

  // test read flatbuffers file
  std::ifstream in("build/test.swmap");
  std::string contents((std::istreambuf_iterator<char>(in)),
                       std::istreambuf_iterator<char>());

  auto level = Map::GetLevel(contents.c_str());

  m_grid->clear();
  std::vector<WVec> vertices;
  float fac{ 15 };
  for (auto&& vert : *level->verts()) {
    vertices.emplace_back(vert->x() * fac, vert->y() * fac);
  }
  for (auto&& face : *level->faces()) {
    ColTriangle tri{ vertices[face->a()],
                     vertices[face->b()],
                     vertices[face->c()] };

    m_grid->lazy_add({ tri.m_center, tri.get_radius(), std::move(tri) });
  }
  m_grid->finish();
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
  m_world.visit([&](const Position& pc) { draw_update(pc, *m_grid); });
}

void
Game::draw()
{
  WRenderer::render_array();
}
