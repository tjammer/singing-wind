#include "collision_system.h"
#include "comps.h"
#include "vec_math.h"
#include "move.h"
#include "static_grid.h"
#include "ecs.hpp"

const float MAX_FLOOR_ANGLE{ 0.7f };

inline bool
is_on_floor(const WVec& normal)
{
  return w_dot(WVec{ 0, -1 }, normal) > MAX_FLOOR_ANGLE;
}

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
    if (is_on_floor(cc.result.normal)) {
      WVec correction = w_slide(WVec(move_back.x, 0), cc.result.normal);
      if (correction.x != 0) {
        correction *= move_back.x / correction.x;
      }
      pc.position -= correction;
    }

    // response
    world.create_component(id, HasCollided{});
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
  auto angle = w_dot(WVec{ 0, -1 }, cc.result.normal);

  if (is_on_floor(cc.result.normal)) {
    mc.timer = 0;
    mc.velocity.y = w_slide(mc.velocity, cc.result.normal).y * 0.5f;
    mc.active_state = MoveState::Run;
  } else if (angle >= 0) {
    const auto& jr = world.get_component<JumpRun>(id);
    if (mc.active_state != MoveState::Wall) {
      mc.active_state = MoveState::Wall;
      mc.velocity = w_slide(mc.velocity, cc.result.normal);
      if (jr.can_wall_jump) {
        mc.timer = 0;
      }
    }
  } else {
    mc.velocity = w_slide(mc.velocity, cc.result.normal);
  }
}
