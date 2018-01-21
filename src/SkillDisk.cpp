#include "SkillDisk.h"
#include "GameWorld.h"
#include "StatusEffectComponent.h"
#include "StatusEffectHitstun.h"
#include "Components.h"
#include "PosComponent.h"
#include "TagComponent.h"
#include "CollisionComponent.h"
#include "HurtBoxComponent.h"
#include "LifeTimeComponent.h"

void
DiskCastMove::accel(GameWorld& world, unsigned int entity)
{
  auto& mc = world.move_c(entity);
  mc.accel -= mc.velocity * c_drag;
}

void
DiskProjectileMove::enter(GameWorld& world, unsigned int entity)
{
  assert(w_magnitude(dir) == 1.0f);
}

void
DiskProjectileMove::accel(GameWorld& world, unsigned int entity)
{
  auto& mc = world.move_c(entity);
  mc.velocity = dir * move_speed;
}

void
DiskProjectileMove::leave(GameWorld& world, unsigned int entity)
{
  world.queue_delete(entity);
}

void
disk_skill_hurtfunc(GameWorld& world, unsigned int victim, unsigned int)
{
  statuseffects::add_effect(world, victim, std::make_shared<Hitstun>(0.1f));
}

void
DiskCastMove::leave(GameWorld& world, unsigned int entity)
{
  auto hurtbox = world.create_entity();
  bset comps;
  for (auto i : { CPosition,
                  CColShape,
                  CDynCol,
                  CStaticCol,
                  CDebugDraw,
                  CTag,
                  CLifeTime,
                  CMove }) {
    comps.set(i);
  }
  world.entities()[hurtbox] = comps;

  world.name_c(hurtbox) = "disk_projectile";

  // pos
  world.pos_c(hurtbox).position = world.pos_c(entity).global_position;
  build_global_transform(world, hurtbox);

  // col shape
  world.cshape_c(hurtbox).shape = std::make_shared<ColCircle>(30);

  // tags
  auto& tc = world.tag_c(hurtbox);
  tc.set(static_cast<int>(Tags::Hurtbox));
  tc.set(static_cast<int>(Tags::Projectile));

  // dyn col
  set_dynamic_col(world.dyn_col_c(hurtbox), DynColResponse::HurtBox);
  // static col
  set_static_col(world.static_col_c(hurtbox),
                 StaticColResponse::DiskProjectile);

  // moveset
  auto& mc = world.move_c(hurtbox);
  mc.special_movestate = std::make_unique<DiskProjectileMove>(
    w_normalize(world.move_c(entity).velocity));

  // hurtbox
  auto& hb = world.hurtbox_c(hurtbox);
  hb = HurtBoxComponent{ entity, {}, disk_skill_hurtfunc, nullptr };

  // lifetime
  auto& lc = world.lifetime_c(hurtbox);
  lc.timer = 30;
}

void
DiskSkill::set_special(MoveComponent& mc)
{
  mc.special_movestate = std::make_unique<DiskCastMove>();
}

void
disk_projectile::on_static_collision(GameWorld& world, unsigned int entity)
{
  auto& mc = world.move_c(entity);
  auto result = world.static_col_c(entity).col_result;
  mc.velocity = w_reflect(mc.velocity, result.normal);
  mc.accel = w_reflect(mc.accel, result.normal);
}
