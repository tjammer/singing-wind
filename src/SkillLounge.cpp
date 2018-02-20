#include "SkillLounge.h"
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
#include "steering.h"
#include "StatusEffectKnockback.h"
#include "StatusEffectHitstun.h"
#include "TagComponent.h"

bool
lounge_skill_hurtfunc(GameWorld& world,
                      unsigned int victim,
                      unsigned int attacker,
                      unsigned int)
{
  // knockback
  movement::interrupt(world, victim);
  auto dir = w_normalize(world.get<PosComponent>(victim).global_position -
                         world.get<PosComponent>(attacker).global_position);
  world.get<MoveComponent>(victim).velocity = dir * 1000.f;
  statuseffects::add_effect(world, victim, std::make_shared<Knockback>(.4f));
  statuseffects::add_effect(world, victim, std::make_shared<Hitstun>(0.1f));
  // TODO: damage
  return true;
}

bool
lounge_skill_on_hit(GameWorld& world,
                    unsigned int attacker,
                    unsigned int,
                    unsigned int)
{
  statuseffects::add_effect(world, attacker, std::make_shared<Hitstun>(0.1f));
  return true;
}

void
create_lounge_hurtbox(GameWorld& world,
                      unsigned int hurtbox,
                      unsigned int parent)
{
  bset comps;
  for (auto i :
       { CPosition, CColShape, CDynCol, CDebugDraw, CTag, CLifeTime }) {
    comps.set(i);
  }

  world.entities()[hurtbox] = comps;
  world.get<NameComponent>(hurtbox).name = "lounge_skill_hurtbox";

  // pos
  float radius = world.get<ColShapeComponent>(parent).shape->get_radius();
  auto& pc = world.get<PosComponent>(hurtbox);
  pc.parent = parent;
  pc.position = { 0, -radius * 0.3f };
  pc.rotation = 0;
  build_global_transform(world, hurtbox);
  // col shape
  auto& csc = world.get<ColShapeComponent>(hurtbox);
  csc.shape = std::make_shared<ColCapsule>(radius * 1.2f, radius * 0.7f);

  // tags
  auto& tc = world.get<TagComponent>(hurtbox);
  tc.tags.reset();
  tc.tags.set(static_cast<int>(Tags::Hurtbox));

  // dyn col
  set_dynamic_col(world.get<DynamicColComponent>(hurtbox),
                  DynColResponse::HurtBox);

  // lifetime
  auto& lc = world.get<LifeTimeComponent>(hurtbox);
  assert(world.get<MoveComponent>(parent).special_movestate != nullptr);
  lc.timer = world.get<MoveComponent>(parent).special_movestate->timer;

  // hurtbox
  auto& hb = world.get<HurtBoxComponent>(hurtbox);
  hb =
    HurtBoxComponent{ parent, {}, lounge_skill_hurtfunc, lounge_skill_on_hit };
}

void
LoungeAttackMove::enter(GameWorld& world, unsigned int entity)
{
  using namespace std::placeholders;
  auto func = std::bind(create_lounge_hurtbox, _1, _2, entity);
  world.queue_create(func);
}

void
LoungeCastMove::accel(GameWorld& world, unsigned int entity)
{
  auto& pc = world.get<PosComponent>(entity);
  auto& ic = world.get<InputComponent>(entity);
  auto& mc = world.get<MoveComponent>(entity);

  rotate_to(ic.mouse.get(), mc.c_max_change_angle * 1.5f, pc);

  // cancel gravity
  mc.accel.y -= c_gravity;
  // slow caster down
  mc.accel -= mc.velocity * 1.5f;
}

void
LoungeAttackMove::accel(GameWorld& world, unsigned int entity)
{
  auto& pc = world.get<PosComponent>(entity);
  auto& mc = world.get<MoveComponent>(entity);
  auto& ic = world.get<InputComponent>(entity);

  float lounge_speed = 2000;
  WVec dir = w_normalize(mc.velocity);

  mc.accel = SteeringBuilder(pc.global_position, mc.velocity, lounge_speed, 1)
               .seek(ic.mouse.get())
               .end(0);
  auto angle =
    w_angle_to_vec(w_rotated(WVec(0, -1), pc.rotation * pc.direction), dir);
  rotate_angle(angle * pc.direction, mc.c_max_change_angle, pc);
  mc.velocity = w_magnitude(mc.velocity) *
                w_rotated(WVec(0, -1), pc.rotation * pc.direction);
}

void
LoungeSkill::set_special(MoveComponent& mc)
{
  mc.special_movestate = std::make_unique<LoungeCastMove>();
}

void
LoungeAfterCastMove::accel(GameWorld& world, unsigned int entity)
{
  auto& pc = world.get<PosComponent>(entity);
  auto& mc = world.get<MoveComponent>(entity);

  auto angle = w_angle_to_vec(
    w_rotated(WVec(0, -1), pc.rotation * pc.direction), mc.velocity);
  rotate_angle(angle * pc.direction, mc.c_max_change_angle, pc);

  // cancel gravity
  mc.accel.y -= c_gravity;
  // slow caster down
  mc.accel -= mc.velocity;
}
