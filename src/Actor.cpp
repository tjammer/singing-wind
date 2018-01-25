#include "Actor.h"
#include "CollisionComponent.h"
#include "GameWorld.h"
#include "MoveSystems.h"
#include "PosComponent.h"
#include "TagComponent.h"
#include "WVecMath.h"

void
actor::on_static_collision(GameWorld& world, unsigned int entity)
{
  auto& mc = world.get<MoveComponent>(entity);
  const auto& result = world.get<StaticColComponent>(entity).col_result;

  if (w_dot(WVec(0, 1), result.normal) > c_max_floor_angle) {
    mc.timer = 0;
    mc.velocity.y = w_slide(mc.velocity, result.normal).y * 0.5f;
  } else {
    mc.velocity = w_slide(mc.velocity, result.normal);
  }
}

void
actor::on_dynamic_collision(GameWorld& world, unsigned int entity)
{
  const auto& collider = world.get<DynamicColComponent>(entity).collided;
  const auto& result = world.get<DynamicColComponent>(entity).col_result;
  auto& mc = world.get<MoveComponent>(entity);
  auto& pc = world.get<PosComponent>(entity);

  const auto& tag = world.get<TagComponent>(collider);
  if (tag.tags.test(static_cast<int>(Tags::Actor))) {
    pc.position -= 0.5f * result.normal * result.depth;
    // pc.global_transform = glm::rotate(glm::translate(WTransform(),
    // pc.position), pc.rotation) * world.pos_c(pc.parent).global_transform;
    // dont slide for now
    world.get<MoveComponent>(collider).additional_force +=
      200.f * result.normal;
    mc.velocity =
      0.98f * mc.velocity + .02f * w_slide(mc.velocity, result.normal);
  }
}
