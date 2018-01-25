#include "SkillMelee.h"
#include "ColShape.h"
#include "CollisionComponent.h"
#include "Components.h"
#include "GameWorld.h"
#include "HurtBoxComponent.h"
#include "InputComponent.h"
#include "LifeTimeComponent.h"
#include "MoveSystems.h"
#include "PosComponent.h"
#include "SkillComponent.h"
#include "StatusEffectComponent.h"
#include "TagComponent.h"
#include "WVecMath.h"
#include "TagComponent.h"
#include "StatusEffectKnockback.h"
#include "StatusEffectHitstun.h"

bool
melee_skill_hurtfunc(GameWorld& world,
                     unsigned int victim,
                     unsigned int attacker,
                     unsigned int)
{
  // knockback
  auto dir = w_normalize(world.get<PosComponent>(victim).global_position -
                         world.get<PosComponent>(attacker).global_position);
  world.get<MoveComponent>(victim).velocity = dir * 400.f;
  statuseffects::add_effect(world, victim, std::make_shared<Knockback>(0.6f));
  statuseffects::add_effect(world, victim, std::make_shared<Hitstun>(0.1f));
  // TODO: damage
  return true;
}

bool
melee_skill_on_hit(GameWorld& world,
                   unsigned int attacker,
                   unsigned int victim,
                   unsigned int)
{
  auto dir = w_normalize(world.get<PosComponent>(attacker).global_position -
                         world.get<PosComponent>(victim).global_position);
  world.get<MoveComponent>(attacker).velocity = dir * 200.f;
  statuseffects::add_effect(world, attacker, std::make_shared<Knockback>(0.1f));
  statuseffects::add_effect(world, attacker, std::make_shared<Hitstun>(0.1f));

  world.get<MoveComponent>(attacker).timer = -0.5f;
  return true;
}

void
MeleeAttackMove::accel(GameWorld& world, unsigned int entity)
{
  const auto& pc = world.get<PosComponent>(entity);
  auto& mc = world.get<MoveComponent>(entity);

  mc.accel = 1500.f * w_rotated(WVec(0, -1), pc.rotation * pc.direction);
}

void
create_hurtbox(GameWorld& world, unsigned int hurtbox, unsigned int parent)
{
  bset comps;
  for (auto i :
       { CPosition, CColShape, CDynCol, CDebugDraw, CTag, CLifeTime }) {
    comps.set(i);
  }

  world.entities()[hurtbox] = comps;
  world.get<NameComponent>(hurtbox).name = "melee_skill_hurtbox";

  // pos
  auto& pc = world.get<PosComponent>(hurtbox);
  pc.parent = parent;
  pc.position = WVec(10, -35);
  pc.rotation = 0;
  build_global_transform(world, hurtbox);
  // col shape
  auto& csc = world.get<ColShapeComponent>(hurtbox);
  csc.shape = std::make_shared<ColCapsule>(45, 25);

  // tags
  auto& tc = world.get<TagComponent>(hurtbox);
  tc.tags.set(static_cast<int>(Tags::Hurtbox));

  // dyn col
  set_dynamic_col(world.get<DynamicColComponent>(hurtbox),
                  DynColResponse::HurtBox);

  // lifetime
  auto& lc = world.get<LifeTimeComponent>(hurtbox);
  lc.timer = .3;

  // hurtbox
  auto& hb = world.get<HurtBoxComponent>(hurtbox);
  hb = HurtBoxComponent{ parent, {}, melee_skill_hurtfunc, melee_skill_on_hit };
}

void
MeleeAttackMove::enter(GameWorld& world, unsigned int entity)
{
  world.queue_create({ create_hurtbox, entity });
}

void
MeleeSkill::set_special(MoveComponent& mc)
{
  mc.special_movestate = std::make_unique<MeleeCastMove>();
}
