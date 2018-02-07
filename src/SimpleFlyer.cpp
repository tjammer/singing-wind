#include "SimpleFlyer.h"
#include "CollisionComponent.h"
#include "GameWorld.h"
#include "InputComponent.h"
#include "MoveSystems.h"
#include "Pathfinding.h"
#include "PosComponent.h"
#include "WVecMath.h"

void
simpleflyer::on_static_collision(GameWorld& world, unsigned int entity)
{
  auto& mc = world.get<MoveComponent>(entity);
  auto result = world.get<StaticColComponent>(entity).col_result;
  mc.velocity = w_slide(mc.velocity, result.normal);
}

void
SimpleFlyingMove::accel(GameWorld& world, unsigned int entity)
{
  auto& ic = world.get<InputComponent>(entity);
  auto& pc = world.get<PosComponent>(entity);
  auto& mc = world.get<MoveComponent>(entity);

  mc.accel = ic.mouse.get() - pc.global_position;

  auto angle = w_angle_to_vec(
    w_rotated(WVec(0, -1), pc.rotation * pc.direction), mc.velocity);
  rotate_angle(angle * pc.direction, mc.c_max_change_angle, pc);
}

void
SimpleFlyingMove::enter(GameWorld& world, unsigned int entity)
{
}

MoveStateName
SimpleFlyingMove::name()
{
  return MoveStateName::SimpleFlying;
}

bool
SimpleFlyingMove::transition(GameWorld& world, unsigned int entity)
{
  // idea is that starts simpleflying if jump is pressed
  if (!world.get<InputComponent>(entity).jump.get()) {
    return true;
  }
  return false;
}

void
HoverMove::enter(GameWorld& world, unsigned int entity)
{
}

void
HoverMove::accel(GameWorld& world, unsigned int entity)
{
  auto& mc = world.get<MoveComponent>(entity);
  auto& pc = world.get<PosComponent>(entity);
  auto& fc = world.get<SimpleFlyComponent>(entity);
  auto& ic = world.get<InputComponent>(entity);

  // rotate
  rotate_angle(pc.direction * pc.rotation, mc.c_max_change_angle, pc);

  // hover
  mc.accel.y = 25.f * (ic.mouse.get().y - pc.global_position.y);
  // add driving force
  if (abs(ic.mouse.get().y - pc.global_position.y) < 2 && mc.velocity.y < 0) {
    mc.accel.y -= 50;
  }
  mc.accel.y -= mc.velocity.y * 0.1f;

  auto xdiff = ic.mouse.get().x - pc.global_position.x;
  mc.accel.x = fmin(xdiff, copysignf(200, xdiff));
  mc.accel.x -= fc.c_stop_coef * mc.velocity.x;
}

MoveStateName
HoverMove::name()
{
  return MoveStateName::Hover;
}

bool
HoverMove::transition(GameWorld& world, unsigned int entity)
{
  if (world.get<InputComponent>(entity).jump.get()) {
    return true;
  }
  return false;
}

std::unique_ptr<MoveState>
SimpleFlyerMoveSet::transition(GameWorld& world, unsigned int entity)
{
  auto& mc = world.get<MoveComponent>(entity);

  if (!mc.movestate) {
    return from_undefined(world, entity);
  }

  switch (mc.movestate->name()) {
    case MoveStateName::SimpleFlying: {
      if (SimpleFlyingMove::transition(world, entity)) {
        return std::make_unique<HoverMove>();
      }
      break;
    }
    case MoveStateName::Hover: {
      if (HoverMove::transition(world, entity)) {
        return std::make_unique<SimpleFlyingMove>();
        break;
      }
    }
    default: {
      return from_undefined(world, entity);
      break;
    }
  }
  return nullptr;
}

void
SimpleFlyerMoveSet::init(GameWorld& world, unsigned int entity)
{
  auto& mc = world.get<MoveComponent>(entity);
  mc.movestate = from_undefined(world, entity);
  mc.special_movestate = nullptr;
}

std::unique_ptr<MoveState>
SimpleFlyerMoveSet::from_undefined(GameWorld&, unsigned int)
{
  return std::make_unique<HoverMove>();
}
