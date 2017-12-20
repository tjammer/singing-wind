#include "StatusEffectComponent.h"
#include "GameWorld.h"
#include "MoveSystems.h"
#include "Components.h"
#include "CollisionComponent.h"
#include <algorithm>
#include "WVecMath.h"

#include <iostream>

namespace statuseffects {
    void add_effect(GameWorld &world, unsigned int entity, StatusEffect& effect) {
        if (!world.entities()[entity].test(CStatusEffect)) {
            return;
        }
        auto &effects = world.statuseffect_c(entity).effects;
        auto it = std::find_if(effects.begin(), effects.end(), [effect](const StatusEffect& e) {return e.id == effect.id;});
        if (it == effects.end()) {
            effects.push_back(effect);
        } else {
            // TODO: check for timing collisions
            *it = effect;
        }
        auto fn = effect.on_start;
        if (fn) {
            fn(world, entity);
        }
    }

    void delete_effect(GameWorld &world, unsigned int entity, StatusEffect &effect) {
        auto &effects = world.statuseffect_c(entity).effects;
        effects.erase(std::remove_if(effects.begin(), effects.end(), [effect](const StatusEffect& e) {return e.id == effect.id;}));
    }
}

void knockback_start(GameWorld &world, unsigned int entity) {
    //auto &mc = world.move_c(entity);
    //mc.special = SpecialMoveState::Knockback;
    assert(false);
}

void knockback_stop(GameWorld &world, unsigned int entity) {
    //auto &mc = world.move_c(entity);
    //if (mc.special == SpecialMoveState::Knockback) {
    //    mc.special = SpecialMoveState::None;
    //}
    assert(false);
}

void statuseffects::knockback_move(GameWorld &world, unsigned int entity) {
    auto &mc = world.move_c(entity);
    mc.accel.y -= c_gravity * 0.5f;
}

StatusEffect statuseffects::knockback() {
    StatusEffect out;
    out.on_start = knockback_start;
    out.on_stop = knockback_stop;
    out.id = StatusEffects::Knockback;
    return out;
}

void hitstun_tick(GameWorld &world, unsigned int entity) {
    world.move_c(entity).time_fac *= 0.1f;
}

StatusEffect statuseffects::hitstun() {
    StatusEffect out;
    out.on_start = hitstun_tick;
    out.on_tick = hitstun_tick;
    out.id = StatusEffects::HitStun;
    return out;
}
