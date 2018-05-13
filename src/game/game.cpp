#include "game.h"
#include "comps.h"
#include "input.h"
#include "input_system.h"
#include "fall.h"
#include "renderer.h"
#include "move.h"
#include "draw.h"
#include "collision_system.h"
#include "flatbuffers/flatbuffers.h"
#include "map_generated.h"
#include "glm/glm.hpp"
#include <fstream>

Game::Game(const WVec& viewport)
  : m_world(ecs::World{})
  , m_frame_timer(FrameTimer{})
  , m_camera(viewport)
  , m_grid(std::make_unique<StaticGrid>())
{
  auto player = m_world.create_entity();
  m_world.create_component(player, Input{});
  m_world.create_component(player, Transform{});
  m_world.create_component(player,
                           Collision{ {}, std::make_unique<Capsule>(20, 10) });
  m_world.create_component(player, Movement{});
  m_world.create_component(player, IsFalling{});

  auto& pc = m_world.get_component<Transform>(player);
  pc.rotation = 0.5;

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
    std::vector<WVec> faceverts;
    for (auto fv : *face->indices()) {
      faceverts.push_back(vertices[fv]);
    }
    Polygon shape{ faceverts };

    m_grid->lazy_add({ shape.center(), shape.radius(), std::move(shape) });
  }
  m_grid->finish();
  m_frame_timer.reset();
}

void
Game::update()
{
  WRenderer::reset();
  WInput::set_mouse(m_camera.unproject_mouse(WInput::get_raw_mouse()));

  m_frame_timer.update();
  while (m_frame_timer.pop_fixed()) {
    m_world.visit(input_update);
    // accel systems
    m_world.visit(fall_update);
    // now integrate
    m_world.visit(
      [](Movement& mc, Transform& pc) { move_update(mc, pc, FIXED_TIMESTEP); });

    // collision
    m_world.visit([&](Collision& cc, Transform& pc, ecs::World::ent_id id) {
      collision_update(cc, pc, m_world, id, *m_grid);
    });
    m_world.visit(on_collision);
  }
  m_world.visit([&](const Transform& pc) { draw_update(pc, *m_grid); });
}

void
Game::draw()
{
  WRenderer::render_array();
}
