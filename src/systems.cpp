//
// Created by tobi on 4/11/17.
//

#include "systems.h"
#include "AIComponent.h"
#include "CPruneSweep.h"
#include "ColGrid.h"
#include "ColShape.h"
#include "CollisionComponent.h"
#include "Components.h"
#include "GameWorld.h"
#include "InputComponent.h"
#include "LifeTimeComponent.h"
#include "MoveSystems.h"
#include "PosComponent.h"
#include "Protagonist.h"
#include "SkillComponent.h"
#include "StatusEffectComponent.h"

#include <WRenderer.h>
#include <WVecMath.h>
#include <algorithm>

void
debug_draw_update(GameWorld& world, const std::vector<unsigned int>& entities)
{
  WTransform zero_tf;
  WRenderer::set_mode(PLines);
  for (const auto& tri : world.grid().get_objects()) {
    tri.shape->add_gfx_lines(zero_tf);
  }

  for (const auto entity : entities) {
    auto& shape = world.cshape_c(entity).shape;
    const auto& transform = world.pos_c(entity).global_transform;
    shape->add_gfx_lines(transform);
  }
}

void
static_col_update(GameWorld& world, const std::vector<unsigned int>& entities)
{

  for (const auto entity : entities) {
    // position
    auto& transform = world.pos_c(entity).global_transform;
    auto& pos = world.pos_c(entity).position;

    // collision
    auto& result = world.static_col_c(entity).col_result;
    auto& shape = world.cshape_c(entity).shape;

    // circle to world
    shape->transform(transform);

    // overwrite result
    result = ColResult();

    auto colliders = world.grid().find_colliders_in_radius(shape->m_center,
                                                           shape->get_radius());
    for (const auto& tri : colliders) {
      auto cr = shape->collides(*tri.shape);
      if (cr.collides) {
        if (cr.depth > result.depth) {
          result = cr;
        }
      }
    }
    shape->reset();

    if (result.collides && result.epa_it < 21) {
      auto move_back = result.normal * -result.depth;
      pos += move_back;

      // player can stand on slopes
      if (w_dot(WVec(0, 1), result.normal) > c_max_floor_angle) {
        WVec correction = w_slide(WVec(move_back.x, 0), result.normal);
        if (correction.x != 0) {
          correction *= move_back.x / correction.x;
        }
        pos -= correction;
      }

      // slide movement and collide again
      // circle to world
      build_global_transform(world, entity);
      shape->transform(transform);

      ColResult second_result;

      for (const auto& tri : colliders) {
        auto cr = shape->collides(*tri.shape);
        if (cr.collides) {
          if (cr.depth > second_result.depth) {
            second_result = cr;
          }
        }
      }
      shape->reset();

      if (second_result.collides) {
        WVec correction =
          find_directed_overlap(second_result, WVec(-move_back.y, move_back.x));
        pos += correction;

        build_global_transform(world, entity);
      }

      // call back
      world.static_col_c(entity).col_response(world, entity);
    }
  }
}

void
input_update(GameWorld& world, const std::vector<unsigned int>& entities)
{
  for (const auto entity : entities) {
    auto& ic = world.input_c(entity);
    switch (ic.input_func) {
      case InputFunc::Protagonist: {
        protagonist::handle_inputs(world, entity);
        break;
      }
      case InputFunc::AI: {
        // should set in tree function
        // world.ai_c(entity).state->do_input(world, entity);
        break;
      }
      default:
        break;
    }
  }
}

void
move_update(GameWorld& world, float timedelta)
{
  for (unsigned int entity = 0; entity < world.entities().size(); ++entity) {

    if (world.entities()[entity].test(CMove)) {
      assert(world.move_c(entity).moveset ||
             world.move_c(entity).special_movestate);

      auto old_accel = world.move_c(entity).accel;
      auto dt = timedelta * world.move_c(entity).time_fac;

      world.move_c(entity).accel = WVec(0, c_gravity);
      world.move_c(entity).accel +=
        world.move_c(entity).additional_force / world.move_c(entity).mass;

      world.move_c(entity).velocity += old_accel * dt;

      // if char is in special state, the correct func needs to be found
      if (world.move_c(entity).special_movestate) {
        world.move_c(entity).special_movestate->accel(world, entity);
        world.move_c(entity).special_movestate->timer -= dt;
        if (world.move_c(entity).special_movestate->timer <= 0) {
          world.move_c(entity).special_movestate->leave(world, entity);
          world.move_c(entity).special_movestate =
            world.move_c(entity).special_movestate->next();
          if (world.move_c(entity).special_movestate) {
            world.move_c(entity).special_movestate->enter(world, entity);
          }
        }
      } else {
        // check transistions
        auto transition =
          world.move_c(entity).moveset->transition(world, entity);
        if (transition) {
          transition->enter(world, entity);
          world.move_c(entity).movestate = std::move(transition);
        }
        world.move_c(entity).movestate->accel(world, entity);
      }

      world.move_c(entity).velocity +=
        dt * (world.move_c(entity).accel - old_accel) / 2.0f;
      auto motion = dt * (world.move_c(entity).velocity +
                          world.move_c(entity).accel * dt / 2.0f);
      world.pos_c(entity).position += motion;
      world.move_c(entity).additional_force = { 0, 0 };
      world.move_c(entity).time_fac = 1;

      world.move_c(entity).timer += dt;
    }

    build_global_transform(world, entity);
  }
}

void
skill_update(GameWorld& world,
             float dt,
             const std::vector<unsigned int>& entities)
{
  for (const auto entity : entities) {
    auto& sc = world.skill_c(entity);
    auto& ic = world.input_c(entity);

    // handle skill input
    if (ic.attacks[0].just_added(true)) {
      skill::cast(world, entity, sc.skills[0]->get_id());
    }

    for (auto& skill : sc.skills) {

      if (skill->state == SkillState::Ready) {
        continue;
      }
      skill->timer -= dt;
      if (skill->timer <= 0) {
        if (skill->state == SkillState::Active) {
          // to cooldown
          skill->timer = skill->get_t_cooldown();
          skill->state = SkillState::Cooldown;
          sc.active = nullptr;
        } else { // should be cooldown
          // to ready
          skill->state = SkillState::Ready;
        }
      }
    }
  }
}

void
dyn_col_update(GameWorld& world, std::vector<unsigned int>& entities)
{
  auto& grid = world.dynamic_grid();
  grid.clear();

  for (const auto& entity : entities) {
    auto& shape = world.cshape_c(entity).shape;
    shape->transform(world.pos_c(entity).global_transform);
    auto colliders = grid.insert_and_find_colliders(
      DynamicEntity{ shape->m_center, shape->get_radius(), entity });
    for (auto& dyn_ent : colliders) {
      auto other = dyn_ent.entity;
      if (world.pos_c(other).parent == entity ||
          world.pos_c(entity).parent == other) {
        continue;
      }
      auto& other_shape = world.cshape_c(other).shape;
      other_shape->transform(world.pos_c(other).global_transform);
      auto result = shape->collides(*other_shape);
      other_shape->reset();

      if (result.collides) {
        auto& dc = world.dyn_col_c(entity);
        dc.col_result = result;
        dc.collided = other;
        if (dc.col_response) {
          dc.col_response(world, entity);
        }
        auto& dcb = world.dyn_col_c(other);
        dcb.col_result = result;
        dcb.col_result.normal *= -1.f;
        dcb.collided = entity;
        if (dcb.col_response) {
          dcb.col_response(world, other);
        }
      }
    }
    shape->reset();
  }
}

void
lifetime_update(GameWorld& world,
                float dt,
                const std::vector<unsigned int>& entities)
{
  for (const auto& entity : entities) {
    auto& lc = world.lifetime_c(entity);
    lc.timer -= dt;
    if (lc.timer <= 0) {
      if (lc.delete_func) {
        lc.delete_func(world, entity);
      }
      world.queue_delete(entity);
    }
  }
}

void
statuseffect_update(GameWorld& world,
                    float dt,
                    const std::vector<unsigned int>& entities)
{
  for (const auto& entity : entities) {
    for (auto& effect : world.statuseffect_c(entity).effects) {
      effect->timer -= dt;

      if (effect->timer <= 0) {
        effect->leave(world, entity);
        statuseffects::delete_effect(world.statuseffect_c(entity), effect);
      } else {
        effect->tick(world, entity);
      }
    }
  }
}

void
ai_update(GameWorld& world, float dt, const std::vector<unsigned int>& entities)
{
  for (const auto& entity : entities) {
    auto& ac = world.ai_c(entity);
    ac.btree.tick();
  }
}
