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
#include "InputComponent.h"
#include "HealthComponent.h"

void
DiskCastMove::accel(GameWorld& world, unsigned int entity)
{
  auto& mc = world.get<MoveComponent>(entity);
  auto temp = mc.c_max_change_angle;
  mc.c_max_change_angle *= 1.3f;
  mc.movestate->accel(world, entity);
  mc.c_max_change_angle = temp;
}

void
DiskProjectileMove::enter(GameWorld& world, unsigned int entity)
{
  assert(w_magnitude(dir) == 1.0f);
}

void
DiskProjectileMove::accel(GameWorld& world, unsigned int entity)
{
  auto& mc = world.get<MoveComponent>(entity);
  mc.accel = { 0, 0 }; // cancel gravity
  mc.velocity = dir * move_speed;
}

void
DiskProjectileMove::leave(GameWorld& world, unsigned int entity)
{
  world.queue_delete(entity);
}

bool
disk_skill_hurtfunc(GameWorld& world,
                    unsigned int victim,
                    unsigned int attacker,
                    unsigned int entity)
{
  auto& state = dynamic_cast<DiskProjectileMove&>(
    *world.get<MoveComponent>(entity).special_movestate);
  if (attacker == victim && !state.collided) {
    return false;
  }
  statuseffects::add_effect(world, victim, std::make_shared<Hitstun>(0.1f));
  statuseffects::add_effect(world, entity, std::make_shared<Hitstun>(0.05f));
  health::damage(world, victim, { 1 });
  return true;
}

void
create_disk(GameWorld& world, unsigned int hurtbox, unsigned int parent)
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
  world.entities()[hurtbox] = comps;

  world.get<NameComponent>(hurtbox).name = "disk_projectile";

  // pos
  world.get<PosComponent>(hurtbox).position =
    world.get<PosComponent>(parent).global_position;
  build_global_transform(world, hurtbox);

  // col shape
  world.get<ColShapeComponent>(hurtbox).shape = std::make_shared<ColCircle>(20);

  // tags
  auto& tc = world.get<TagComponent>(hurtbox);
  tc.tags.reset();
  tc.tags.set(static_cast<int>(Tags::Hurtbox));
  tc.tags.set(static_cast<int>(Tags::Projectile));

  // dyn col
  set_dynamic_col(world.get<DynamicColComponent>(hurtbox),
                  DynColResponse::HurtBox);
  // static col
  set_static_col(world.get<StaticColComponent>(hurtbox),
                 StaticColResponse::DiskProjectile);

  // moveset
  auto& mc = world.get<MoveComponent>(hurtbox);
  mc.special_movestate = std::make_unique<DiskProjectileMove>(
    w_rotated(WVec{ 0, -1 },
              world.get<PosComponent>(parent).rotation *
                world.get<PosComponent>(parent).direction));
  mc.moveset = std::make_unique<TimedOnlyMoveSet>();

  // hurtbox
  auto& hb = world.get<HurtBoxComponent>(hurtbox);
  hb = HurtBoxComponent{ parent, {}, disk_skill_hurtfunc, nullptr };

  // lifetime
  auto& lc = world.get<LifeTimeComponent>(hurtbox);
  lc.timer = 3;

  // status effects
  world.get<StatusEffectComponent>(hurtbox).effects.clear();
}

void
DiskCastMove::leave(GameWorld& world, unsigned int entity)
{
  world.queue_create({ create_disk, entity });
  auto& mc = world.get<MoveComponent>(entity);
  auto dir = w_rotated(WVec{ 0, -1 },
                       world.get<PosComponent>(entity).rotation *
                         world.get<PosComponent>(entity).direction);
  mc.accel += dir * 10000.0f;
}

void
DiskSkill::set_special(MoveComponent& mc)
{
  mc.special_movestate = std::make_unique<DiskCastMove>();
}

void
disk_projectile::on_static_collision(GameWorld& world, unsigned int entity)
{
  auto& mc = world.get<MoveComponent>(entity);
  auto& result = world.get<StaticColComponent>(entity).col_result;
  auto& state = dynamic_cast<DiskProjectileMove&>(*mc.special_movestate);
  mc.velocity = w_reflect(mc.velocity, result.normal);
  mc.accel = w_reflect(mc.accel, result.normal);
  state.set_dir(w_normalize(mc.velocity));
  state.collided = true;
}
