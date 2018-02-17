#include "SkillSixPattern.h"
#include "GameWorld.h"
#include "MoveSystems.h"
#include "StatusEffectComponent.h"
#include "StatusEffectHitstun.h"
#include "HealthComponent.h"
#include "PosComponent.h"
#include "TagComponent.h"
#include "CollisionComponent.h"
#include "HurtBoxComponent.h"
#include "WVecMath.h"
#include "ColShape.h"
#include "LifeTimeComponent.h"

void
SixPatternAfterCastMove::accel(GameWorld& world, unsigned int entity)
{
  auto& mc = world.get<MoveComponent>(entity);
  mc.accel = -0.7f * mc.velocity;
}

void
SixPatternCastMove::accel(GameWorld& world, unsigned int entity)
{
  auto& mc = world.get<MoveComponent>(entity);
  // cancel gravity and slow down
  mc.accel = -0.7f * mc.velocity;
}

void
PatternProjectileMove::enter(GameWorld& world, unsigned int entity)
{
  assert(w_magnitude(dir) == 1.0f);
}

void
PatternProjectileMove::accel(GameWorld& world, unsigned int entity)
{
  auto& mc = world.get<MoveComponent>(entity);
  mc.accel = { 0, 0 }; // cancel gravity
  mc.velocity = dir * move_speed;
}

void
PatternProjectileMove::leave(GameWorld& world, unsigned int entity)
{
  world.queue_delete(entity);
}

bool
pattern_skill_hurtfunc(GameWorld& world,
                       unsigned int victim,
                       unsigned int attacker,
                       unsigned int entity)
{
  if (attacker == victim || world.get<TagComponent>(victim).tags.test(
                              static_cast<int>(Tags::Enemy))) {
    return false;
  }
  movement::interrupt(world, victim);
  statuseffects::add_effect(world, victim, std::make_shared<Hitstun>(0.1f));
  health::damage(world, victim, { 1 });
  world.queue_delete(entity);
  return true;
}

void
create_pattern_projectile(GameWorld& world,
                          unsigned int proj,
                          unsigned int parent,
                          WVec dir)
{
  bset comps;
  for (auto i : { CPosition,
                  CColShape,
                  CDynCol,
                  CStaticCol,
                  CDebugDraw,
                  CTag,
                  CLifeTime,
                  CMove,
                  CStatusEffect }) {
    comps.set(i);
  }
  world.entities()[proj] = comps;

  world.get<NameComponent>(proj).name = "pattern_projectile";
  // pos
  world.get<PosComponent>(proj).position =
    world.get<PosComponent>(parent).global_position;
  build_global_transform(world, proj);

  // col shape
  world.get<ColShapeComponent>(proj).shape = std::make_shared<ColCircle>(20);

  // tags
  auto& tc = world.get<TagComponent>(proj);
  tc.tags.reset();
  tc.tags.set(static_cast<int>(Tags::Hurtbox));
  tc.tags.set(static_cast<int>(Tags::Projectile));

  // dyn col
  set_dynamic_col(world.get<DynamicColComponent>(proj),
                  DynColResponse::HurtBox);
  // static col
  set_static_col(world.get<StaticColComponent>(proj),
                 StaticColResponse::DeleteAfterCol);

  // moveset
  auto& mc = world.get<MoveComponent>(proj);
  mc.special_movestate = std::make_unique<PatternProjectileMove>(dir, 500);
  mc.moveset = std::make_unique<TimedOnlyMoveSet>();

  // proj
  auto& hb = world.get<HurtBoxComponent>(proj);
  hb = HurtBoxComponent{ parent, {}, pattern_skill_hurtfunc, nullptr };

  // lifetime
  auto& lc = world.get<LifeTimeComponent>(proj);
  lc.timer = 3;

  // status effects
  world.get<StatusEffectComponent>(proj).effects.clear();
}

void
SixPatternCastMove::leave(GameWorld& world, unsigned int entity)
{
  using namespace std::placeholders;
  const auto& pc = world.get<PosComponent>(entity);
  float start_angle = pc.direction * pc.rotation;

  for (size_t i = 0; i < 6; ++i) {
    float angle = start_angle + (float)i * 1.0f / 6.0f * 2.0f * (float)M_PI;
    auto func = std::bind(
      create_pattern_projectile, _1, _2, _3, w_rotated({ 0, -1 }, angle));
    world.queue_create({ func, entity });
  }
}

void
SixPatternSkill::set_special(MoveComponent& mc)
{
  mc.special_movestate = std::make_unique<SixPatternCastMove>();
}
