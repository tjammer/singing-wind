#include "TestEnemy.h"
#include "GameWorld.h"
#include "WVecMath.h"
#include "Components.h"

#include <iostream>
using namespace TestEnemy;

void TestEnemy::on_static_collision(const ColResult &result, GameWorld &world, unsigned int entity) {
    auto &mc = world.move_c(entity);
    mc.velocity = w_slide(mc.velocity, result.normal);
}

void TestEnemy::handle_inputs(GameWorld &world, unsigned int entity) {
    auto &ic = world.input_c(entity);

    unsigned int following = 1; //should be player

    push_value(ic.mouse, world.pos_c(following).position);
}

void TestEnemy::simple_flying(GameWorld &world, unsigned int entity) {
    auto &ic = world.input_c(entity);
    auto &fc = world.simple_fly_c(entity);
    auto &pc = world.pos_c(entity);
    auto &mc = world.move_c(entity);
    
    auto dir = w_normalize(ic.mouse[0] - pc.position);
    // cancel gravity
    mc.accel.y = 0;
    float distance = w_magnitude(ic.mouse[0] - pc.position);
    if (distance > fc.c_near_threshold) {
        // accel if not already max vel
        if (w_magnitude(mc.velocity) >= fc.c_max_vel) {
            mc.velocity = w_normalize(mc.velocity) * fc.c_max_vel * 0.5f + dir * fc.c_max_vel * 0.5f;
        }
        mc.accel += fc.c_accel * dir;
    } else {
        mc.accel -= fc.c_stop_coef * mc.velocity;
    }
}
