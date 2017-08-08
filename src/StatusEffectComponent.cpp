#include "StatusEffectComponent.h"
#include "GameWorld.h"
#include "MoveSystems.h"
#include "Components.h"
#include "CollisionComponent.h"
#include <algorithm>
#include "WVecMath.h"

#include <iostream>

void add_effect(GameWorld &world, unsigned int entity, StatusEffect& effect) {
    if (!world.entities()[entity].test(CStatusEffect)) {
        return;
    }
    auto &effects = world.statuseffect_c(entity).effects;
    auto it = std::find_if(effects.begin(), effects.end(), [effect](const StatusEffect& e) {return e.id == effect.id;});
    if (it == effects.end()) {
        effects.push_back(effect);
    } else {
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

void knockback_start(GameWorld &world, unsigned int entity) {
    auto &effects = world.statuseffect_c(entity).effects;
    auto it = std::find_if(effects.begin(), effects.end(), [](const StatusEffect& e) {return e.id == StatusEffects::Knockback;});
    assert(it != effects.end());
    StatusEffect &effect = *it;
    auto &mc = world.move_c(entity);
    effect.saved_state.push_back(static_cast<int>(mc.moveset));
    effect.saved_state.push_back(static_cast<int>(mc.movestate));
    mc.movestate = MoveState::Falling;
    mc.moveset = MoveSet::Special;
}

void knockback_stop(GameWorld &world, unsigned int entity) {
    auto &mc = world.move_c(entity);
    auto &effects = world.statuseffect_c(entity).effects;
    auto it = std::find_if(effects.begin(), effects.end(), [](const StatusEffect& e) {return e.id == StatusEffects::Knockback;});
    assert(it != effects.end());
    StatusEffect &effect = *it;
    if (mc.moveset == MoveSet::Special and mc.movestate == MoveState::Falling) {
        mc.moveset = static_cast<MoveSet>(effect.saved_state[0]);
        mc.movestate = static_cast<MoveState>(effect.saved_state[1]);
    }
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
