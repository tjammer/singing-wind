#include "TestEnemy.h"
#include "GameWorld.h"
#include "WVecMath.h"
#include "Components.h"

using namespace TestEnemy;

void TestEnemy::handle_inputs(GameWorld &world, unsigned int entity) {
    auto &ic = world.input_c(entity);

    unsigned int following = 1; //should be player

    if (world.pos_c(following).position.x > world.pos_c(entity).position.x) {
        push_value(ic.direction, 1);
    }
    else {
        push_value(ic.direction, -1);
    }
}
