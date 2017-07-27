#include "HurtBoxComponent.h"
#include "GameWorld.h"

void hurtbox::on_dynamic_collision(GameWorld &world, const unsigned int entity) {
    auto fn = world.hurtbox_c(entity).hit_function;
    if (fn) {
        fn(world, entity);
    }
}
