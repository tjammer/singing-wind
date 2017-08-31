#include "Lounge.h"
#include "GameWorld.h"
#include "Components.h"
#include "PosComponent.h"
#include "InputComponent.h"
#include "CollisionComponent.h"
#include "ColShape.h"
#include "TagComponent.h"
#include "SkillComponent.h"
#include "LifeTimeComponent.h"
#include "HurtBoxComponent.h"
#include "MoveSystems.h"
#include "WVecMath.h"
#include "Components.h"
#include "StatusEffectComponent.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_transform_2d.hpp>
#include "StatusEffectComponent.h"

void lounge_skill_hurtfunc(GameWorld &world, unsigned int victim, unsigned int attacker) {
    // knockback
    auto dir = w_normalize(world.pos_c(victim).position - world.pos_c(attacker).position);
    world.move_c(victim).velocity = dir * 1000.f;
    auto kb = statuseffects::knockback();
    kb.timer = .4f;
    statuseffects::add_effect(world, victim, kb);
    auto hs = statuseffects::hitstun();
    hs.timer = .1f;
    statuseffects::add_effect(world, victim, hs);
    // TODO: damage
}

void lounge_skill_on_hit(GameWorld &world, unsigned int attacker, unsigned int victim) {
    auto dir = w_normalize(world.pos_c(attacker).position - world.pos_c(victim).position);
    world.move_c(attacker).velocity = dir * 1000.f;
    auto kb = statuseffects::knockback();
    kb.timer = .1f;
    statuseffects::add_effect(world, attacker, kb);
    auto hs = statuseffects::hitstun();
    hs.timer = .1f;
    statuseffects::add_effect(world, attacker, hs);
}

namespace lounge_skill {

    void Skill::channel_start(GameWorld &world, unsigned int entity) {
        // set movestate for caster
        auto &mc = world.move_c(entity);
        mc.special = SpecialMoveState::LoungeChannel;

        // create hurtbox
        auto hurtbox = world.create_entity();
        bset comps;
        for (auto i : {CPosition, CColShape, CDynCol, CDebugDraw, CTag, CLifeTime}) {
            comps.set(i);
        }

        world.entities()[hurtbox] = comps;
        world.name_c(hurtbox) = "lounge_skill_hurtbox";

        // pos
        auto &pc = world.pos_c(hurtbox);
        pc.parent = entity;
        pc.position = WVec(0, 0);
        pc.rotation = 0;
        pc.global_transform =  world.pos_c(pc.parent).global_transform * glm::rotate(glm::translate(WTransform(), pc.position), pc.rotation);
        // col shape
        auto &csc = world.cshape_c(hurtbox);
        csc.shape = world.cshape_c(entity).shape;

        // tags
        auto &tc = world.tag_c(hurtbox);
        tc.set(static_cast<int>(Tags::Hurtbox));

        // dyn col
        auto &dc = world.dyn_col_c(hurtbox);
        dc.col_response = DynColResponse::HurtBox;

        // lifetime
        auto &lc = world.lifetime_c(hurtbox);
        lc.timer = c_time_channel;

        // hurtbox
        auto &hb = world.hurtbox_c(hurtbox);
        hb.owner = entity;
        hb.hurt_function = lounge_skill_hurtfunc;
        hb.on_hit = lounge_skill_on_hit;
    }

    void Skill::channel_end(GameWorld &world, unsigned int entity) {
        reset_special(world, entity, SpecialMoveState::LoungeChannel);
    }

    void Skill::buildup_start(GameWorld &world, unsigned int entity) {
        // set movestate for caster
        auto &mc = world.move_c(entity);
        mc.special = SpecialMoveState::LoungeBuildUp;
    }

    void Skill::buildup_end(GameWorld &world, unsigned int entity) {
        reset_special(world, entity, SpecialMoveState::LoungeBuildUp);
    }

    void Skill::recover_start(GameWorld &world, unsigned int entity) {
        // set movestate for caster
        auto &mc = world.move_c(entity);
        mc.special = SpecialMoveState::LoungeRecover;
    }

    void Skill::recover_end(GameWorld &world, unsigned int entity) {
        reset_special(world, entity, SpecialMoveState::LoungeRecover);
    }

    void move_buildup(GameWorld &world, unsigned int entity) {
        auto &pc = world.pos_c(entity);
        auto &ic = world.input_c(entity);
        auto &fc = world.fly_c(entity);
        auto &mc = world.move_c(entity);

        auto mouse = WVec(glm::inverse(pc.global_transform) * WVec3(ic.mouse[0], 1));
        // see src/Protagonist angle_up
        float mouse_angle = atan2f(mouse.x, -mouse.y);
        pc.rotation += copysignf(fmin(fc.c_max_change_angle, abs(mouse_angle)), mouse_angle);
        pc.rotation = std::remainder(pc.rotation, (float)M_PI * 2.f);

        mc.velocity = {0, 0};
        mc.accel = {0, 0};
    }

    void move_channel(GameWorld &world, unsigned int entity) {
        auto &pc = world.pos_c(entity);
        auto &mc = world.move_c(entity);

        float lounge_speed = 1200;
        float lounge_accel = 5000;

        // diminishing accel, see protagonist/walk
        float vel = fmin(lounge_speed, w_magnitude(mc.velocity));
        mc.accel = w_rotated_deg(WVec(0, -lounge_accel), pc.rotation);
        mc.accel *= (1.f - exp(-pow(vel - lounge_speed, 2.f) * 0.1f/lounge_speed));
    }
}

lounge_skill::Skill::Skill() : SkillBase(0.5, 0.8, 0, 5, SkillID::Lounge) {}
