//
// Created by tobi on 4/11/17.
//

#include "systems.h"
#include "AIComponent.h"
#include "CPruneSweep.h"
#include "StaticGrid.h"
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
#include "HealthComponent.h"

#include <WRenderer.h>
#include <WVecMath.h>
#include <algorithm>
#include <set>

void
debug_draw_update(GameWorld& world, const std::vector<unsigned int>& entities)
{
  WTransform zero_tf;
  WRenderer::set_mode(PLines);
  for (const auto& tri : world.grid().get_objects()) {
    tri.shape->add_gfx_lines(zero_tf);
  }

  for (const auto entity : entities) {
    auto& shape = world.get<ColShapeComponent>(entity).shape;
    const auto& transform = world.get<PosComponent>(entity).global_transform;
    shape->add_gfx_lines(transform);
  }
}

void
static_col_update(GameWorld& world, const std::vector<unsigned int>& entities)
{

  for (const auto entity : entities) {
    // position
    auto& transform = world.get<PosComponent>(entity).global_transform;
    auto& pos = world.get<PosComponent>(entity).position;

    // collision
    auto& result = world.get<StaticColComponent>(entity).col_result;
    auto& shape = world.get<ColShapeComponent>(entity).shape;

    // circle to world
    shape->transform(transform);

    // overwrite result
    result = world.grid().test_against_grid(shape);

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

      auto second_result = world.grid().test_against_grid(shape);

      shape->reset();

      if (second_result.collides) {
        WVec correction =
          find_directed_overlap(second_result, WVec(-move_back.y, move_back.x));
        pos += correction;

        build_global_transform(world, entity);
      }

      // call back
      world.get<StaticColComponent>(entity).col_response(world, entity);
    }
  }
}

void
input_update(GameWorld& world, const std::vector<unsigned int>& entities)
{
  for (const auto entity : entities) {
    auto& ic = world.get<InputComponent>(entity);
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
      assert(world.get<MoveComponent>(entity).moveset ||
             world.get<MoveComponent>(entity).special_movestate);

      auto& mc = world.get<MoveComponent>(entity);

      auto old_accel = mc.accel;
      auto dt = timedelta * mc.time_fac;

      mc.accel = WVec(0, c_gravity);
      mc.accel += mc.additional_force / mc.mass;

      mc.velocity += old_accel * dt;

      // if char is in special state, the correct func needs to be found
      if (mc.special_movestate) {
        mc.special_movestate->accel(world, entity);
        mc.special_movestate->timer -= dt;
        if (mc.special_movestate->timer <= 0) {
          mc.special_movestate->leave(world, entity);
          mc.special_movestate = mc.special_movestate->next();
          if (mc.special_movestate) {
            mc.special_movestate->enter(world, entity);
          }
        }
      } else {
        // check transistions
        auto transition = mc.moveset->transition(world, entity);
        if (transition) {
          transition->enter(world, entity);
          mc.movestate = std::move(transition);
        }
        mc.movestate->accel(world, entity);
      }

      mc.velocity += dt * (mc.accel - old_accel) / 2.0f;
      auto motion = dt * (mc.velocity + mc.accel * dt / 2.0f);
      world.get<PosComponent>(entity).position += motion;
      mc.additional_force = { 0, 0 };
      mc.time_fac = 1;

      mc.timer += dt;
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
    auto& sc = world.get<SkillComponent>(entity);
    auto& ic = world.get<InputComponent>(entity);

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
          assert(skill->state == SkillState::Cooldown);
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
  auto& prune_sweep = world.prune_sweep();
  auto& boxes = prune_sweep.get_boxes();
  std::set<unsigned int> checked;

  for (auto& box : boxes) {
    // has dyn_col_comp
    auto& shape = world.get<ColShapeComponent>(box.entity).shape;
    checked.insert(box.entity);
    if (!shape) {
      continue;
    }
    shape->transform(world.get<PosComponent>(box.entity).global_transform);
    box.mins = shape->m_center - shape->get_radius();
    box.maxs = shape->m_center + shape->get_radius();
    shape->reset();
  }

  for (const auto& entity : entities) {
    if (checked.count(entity) == 0) {
      auto& shape = world.get<ColShapeComponent>(entity).shape;
      const auto& pos = world.get<PosComponent>(entity).position;
      boxes.emplace_back(
        PSBox{ pos - shape->get_radius(), pos + shape->get_radius(), entity });
    } else {
      checked.erase(entity);
    }
  }
  // todo remove boxes from delete list
  for (auto ent : checked) {
    boxes.erase(
      std::remove_if(boxes.begin(), boxes.end(), [ent](const PSBox& b) {
        return b.entity == ent;
      }));
  }

  // prune and sweep
  prune_sweep.prune_and_sweep();

  for (auto& pair : prune_sweep.get_pairs()) {
    unsigned int a = pair.first;
    unsigned int b = pair.second;

    auto& pos_a = world.get<PosComponent>(a);
    auto& pos_b = world.get<PosComponent>(b);

    // for hurtboxes or alert circles
    if ((pos_a.parent == b || pos_b.parent == a)) {
      continue;
    }

    auto& shape_a = world.get<ColShapeComponent>(a).shape;
    auto& shape_b = world.get<ColShapeComponent>(b).shape;

    shape_a->transform(world.get<PosComponent>(a).global_transform);
    shape_b->transform(world.get<PosComponent>(b).global_transform);
    auto result = shape_a->collides(*shape_b);
    shape_a->reset();
    shape_b->reset();

    if (result.collides) {
      auto& dc = world.get<DynamicColComponent>(a);
      dc.col_result = result;
      dc.collided = b;
      if (dc.col_response) {
        dc.col_response(world, a);
      }
      auto& dcb = world.get<DynamicColComponent>(b);
      dcb.col_result = result;
      dcb.col_result.normal *= -1.f;
      dcb.collided = a;
      if (dcb.col_response) {
        dcb.col_response(world, b);
      }
    }
  }
}

void
lifetime_update(GameWorld& world,
                float dt,
                const std::vector<unsigned int>& entities)
{
  for (const auto& entity : entities) {
    auto& lc = world.get<LifeTimeComponent>(entity);
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
  std::vector<std::pair<std::shared_ptr<StatusEffect>, unsigned int>> to_delete;
  for (const auto& entity : entities) {
    for (auto& effect : world.get<StatusEffectComponent>(entity).effects) {
      effect->timer -= dt;

      if (effect->timer <= 0) {
        effect->leave(world, entity);
        // statuseffects::delete_effect(world.get<StatusEffectComponent>(entity),
        //                             effect);
        to_delete.push_back({ effect, entity });
      } else {
        effect->tick(world, entity);
      }
    }
  }
  for (auto& pr : to_delete) {
    statuseffects::delete_effect(world.get<StatusEffectComponent>(pr.second),
                                 pr.first);
  }
}

void
ai_update(GameWorld& world, float dt, const std::vector<unsigned int>& entities)
{
  for (const auto& entity : entities) {
    auto& ac = world.get<AIComponent>(entity);
    ac.btree.tick();
  }
}

void
health_update(GameWorld& world, const std::vector<unsigned int>& entities)
{
  for (const auto& entity : entities) {
    auto& hc = world.get<HealthComponent>(entity);
    if (hc.health <= 0) {
      world.queue_delete(entity);
    }
  }
}
