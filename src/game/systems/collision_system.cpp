#include "collision_system.h"
#include "comps.h"
#include "vec_math.h"
#include "move.h"
#include "static_grid.h"
#include "ecs.hpp"

const float MAX_FLOOR_ANGLE{ 0.7f };

void
collision_update(Collision& cc,
                 Transform& pc,
                 ecs::World& world,
                 std::size_t id,
                 StaticGrid& grid)
{
  cc.shape->transform(pc);

  // overwrite result
  cc.result = grid.test_against_grid(cc.shape, pc);

  if (cc.result.collides) {
    auto move_back = cc.result.normal * -cc.result.depth;
    pc.position += move_back;

    // player can stand on slopes
    if (w_dot(WVec(0, -1), cc.result.normal) > MAX_FLOOR_ANGLE) {
      WVec correction = w_slide(WVec(move_back.x, 0), cc.result.normal);
      if (correction.x != 0) {
        correction *= move_back.x / correction.x;
      }
      pc.position -= correction;
    }

    // slide movement and collide again
    // circle to world

    // cc.shape->transform(pc.global_transform);

    // auto second_result = grid.test_against_grid(cc.shape);

    // cc.shape->reset();

    // if (second_result.collides) {
    //  WVec correction =
    //    find_directed_overlap(second_result, WVec(-move_back.y, move_back.x));
    //  pc.position += correction;

    //  build_global_transform(pc);
    //}

    // call back
    world.create_component(id, HasCollided{});
    // world.get<StaticColComponent>(entity).col_response(world, entity);
  }
}

void
on_collision(HasCollided,
             Movement& mc,
             Collision& cc,
             ecs::World& world,
             std::size_t id)
{
  world.destroy_component<HasCollided>(id);
  if (w_dot(WVec(0, -1), cc.result.normal) > MAX_FLOOR_ANGLE) {
    mc.timer = 0;
    mc.velocity.y = w_slide(mc.velocity, cc.result.normal).y * 0.5f;
    mc.active_state = MoveState::Run;
  } else {
    mc.velocity = w_slide(mc.velocity, cc.result.normal);
  }
}
