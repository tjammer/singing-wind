#include "PosComponent.h"
#include "GameWorld.h"
#include "WindDefs.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_transform_2d.hpp>

void build_global_transform(GameWorld &world, unsigned int entity) {
    using namespace glm;
    auto &pc = world.pos_c(entity);
    pc.global_transform = rotate(translate(WTransform(), pc.position), pc.rotation) * world.pos_c(pc.parent).global_transform;
    // TODO: apply scale for direction
}
