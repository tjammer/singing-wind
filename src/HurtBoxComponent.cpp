#include "HurtBoxComponent.h"
#include "GameWorld.h"
#include "CollisionComponent.h"
#include "TagComponent.h"

#include <algorithm>

void hurtbox::on_dynamic_collision(GameWorld &world, const unsigned int entity) {
    auto &dc = world.dyn_col_c(entity);
    auto &hb = world.hurtbox_c(entity);
    
    if (std::find(hb.hit_entities.begin(), hb.hit_entities.end(), dc.collided) != hb.hit_entities.end()) {
        return;
    // only on Actors
    } else if (!world.tag_c(dc.collided).test(static_cast<int>(Tags::Actor))) {
        return;
    } else if (dc.collided == hb.owner) {
        assert(false);
        return;
    }

    auto fn = hb.hurt_function;
    if (fn) {
        fn(world, dc.collided, hb.owner);
    }
    // we ahve a hit! check if owner has made a response yet
    if (std::find(hb.hit_entities.begin(), hb.hit_entities.end(), hb.owner) == hb.hit_entities.end()) {
        fn = hb.on_hit;
        if (fn) {
            fn(world, hb.owner, dc.collided);
        }
        hb.hit_entities.push_back(hb.owner);
    }
    
    
    hb.hit_entities.push_back(dc.collided);
}
