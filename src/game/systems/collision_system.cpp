#include "collision_system.h"
#include "col_shape.h"
#include "comps.h"
#include "vec_math.h"
#include "move.h"
#include "static_grid.h"

const float MAX_FLOOR_ANGLE{ 0.7f };

void
collision_update(Collision& cc, Position& pc, StaticGrid<StaticTriangle>& grid)
{
  // circle to world
  cc.shape->transform(pc.global_transform);

  // overwrite result
  cc.result = grid.test_against_grid(cc.shape);

  cc.shape->reset();

  if (cc.result.collides && cc.result.epa_it < 21) {
    auto move_back = cc.result.normal * -cc.result.depth;
    pc.position += move_back;

    // player can stand on slopes
    if (w_dot(WVec(0, 1), cc.result.normal) > MAX_FLOOR_ANGLE) {
      WVec correction = w_slide(WVec(move_back.x, 0), cc.result.normal);
      if (correction.x != 0) {
        correction *= move_back.x / correction.x;
      }
      pc.position -= correction;
    }

    // slide movement and collide again
    // circle to world
    build_global_transform(pc);
    cc.shape->transform(pc.global_transform);

    auto second_result = grid.test_against_grid(cc.shape);

    cc.shape->reset();

    if (second_result.collides) {
      WVec correction =
        find_directed_overlap(second_result, WVec(-move_back.y, move_back.x));
      pc.position += correction;

      build_global_transform(pc);
    }

    // call back
    // world.get<StaticColComponent>(entity).col_response(world, entity);
  }
}
