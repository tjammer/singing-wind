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
#include "StatusEffectKnockback.h"
#include "StatusEffectHitstun.h"

void
melee_skill_hurtfunc(GameWorld& world,
                     unsigned int victim,
                     unsigned int attacker)
{
  // knockback
  auto dir = w_normalize(world.pos_c(victim).global_position -
                         world.pos_c(attacker).global_position);
  world.move_c(victim).velocity = dir * 400.f;
  statuseffects::add_effect(world, victim, std::make_shared<Knockback>(0.6f));
  statuseffects::add_effect(world, victim, std::make_shared<Hitstun>(0.1f));
  // TODO: damage
}

void
melee_skill_on_hit(GameWorld& world, unsigned int attacker, unsigned int victim)
{
  auto dir = w_normalize(world.pos_c(attacker).global_position -
                         world.pos_c(victim).global_position);
  world.move_c(attacker).velocity = dir * 200.f;
  statuseffects::add_effect(world, attacker, std::make_shared<Knockback>(0.1f));
  statuseffects::add_effect(world, attacker, std::make_shared<Hitstun>(0.1f));

  world.move_c(attacker).timer = -0.5f;
}

void
MeleeAttackMove::accel(GameWorld& world, unsigned int entity)
{
  const auto& pc = world.pos_c(entity);
  auto& mc = world.move_c(entity);

  mc.accel = 1500.f * w_rotated(WVec(0, -1), pc.rotation * pc.direction);
}

void
MeleeAttackMove::enter(GameWorld& world, unsigned int entity)
{
  // create hurtbox
  auto hurtbox = world.create_entity();
  bset comps;
  for (auto i :
       { CPosition, CColShape, CDynCol, CDebugDraw, CTag, CLifeTime }) {
    comps.set(i);
  }

  world.entities()[hurtbox] = comps;
  world.name_c(hurtbox) = "melee_skill_hurtbox";

  // pos
  auto& pc = world.pos_c(hurtbox);
  pc.parent = entity;
  pc.position = WVec(10, -35);
  pc.rotation = 0;
  build_global_transform(world, hurtbox);
  // col shape
  auto& csc = world.cshape_c(hurtbox);
  csc.shape = std::make_shared<ColCapsule>(45, 25);

  // tags
  auto& tc = world.tag_c(hurtbox);
  tc.set(static_cast<int>(Tags::Hurtbox));

  // dyn col
  set_dynamic_col(world.dyn_col_c(hurtbox), DynColResponse::HurtBox);

  // lifetime
  auto& lc = world.lifetime_c(hurtbox);
  lc.timer = .3;

  // hurtbox
  auto& hb = world.hurtbox_c(hurtbox);
  hb = HurtBoxComponent{ entity, {}, melee_skill_hurtfunc, melee_skill_on_hit };
}

void
MeleeSkill::set_special(MoveComponent& mc)
{
  mc.special_movestate = std::make_unique<MeleeCastMove>();
}
