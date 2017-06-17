#include "TestEnemy.h"
#include "GameWorld.h"
#include "WVecMath.h"

using namespace TestEnemy;

void TestEnemy::handle_inputs(GameWorld &world, unsigned int entity) {
    auto &ic = world.m_input_c.at(entity);

    unsigned int following = 1; //should be player

    if (world.m_pos_c.at(following).position.x > world.m_pos_c.at(entity).position.x) {
        push_value(ic.direction, 1);
    }
    else {
        push_value(ic.direction, -1);
    }
}
