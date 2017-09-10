#include "PosComponent.h"
#include "GameWorld.h"
#include "WindDefs.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_transform_2d.hpp>

void build_global_transform(GameWorld &world, unsigned int entity) {
    using namespace glm;
    auto &pc = world.pos_c(entity);
    assert (abs(pc.direction) == 1);
    pc.global_transform = world.pos_c(pc.parent).global_transform * rotate(scale(translate(WTransform(), pc.position), WVec(pc.direction, 1)), pc.rotation);
}
