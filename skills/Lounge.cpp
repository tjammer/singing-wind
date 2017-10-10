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
#include "steering.h"

void lounge_skill_hurtfunc(GameWorld &world, unsigned int victim, unsigned int attacker) {
    // knockback
    interrupt(world, victim);
    auto dir = w_normalize(world.pos_c(victim).global_position - world.pos_c(attacker).global_position);
    world.move_c(victim).velocity = dir * 1000.f;
    auto kb = statuseffects::knockback();
    kb.timer = .4f;
    statuseffects::add_effect(world, victim, kb);
    auto hs = statuseffects::hitstun();
    hs.timer = .05f;
    statuseffects::add_effect(world, victim, hs);
    // TODO: damage
}

void lounge_skill_on_hit(GameWorld &world, unsigned int attacker, unsigned int) {
    auto hs = statuseffects::hitstun();
    hs.timer = .05f;
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
        float radius = world.cshape_c(entity).shape->get_radius();
        auto &pc = world.pos_c(hurtbox);
        pc.parent = entity;
        pc.position = WVec(0, -radius * 0.3f);
        pc.rotation = 0;
        build_global_transform(world, hurtbox);
        // col shape
        auto &csc = world.cshape_c(hurtbox);
        csc.shape = std::shared_ptr<ColShape>(new ColCapsule(radius * 1.2f, radius * 0.7f));

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
        hb.hit_entities.clear();
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
        auto &mc = world.move_c(entity);

        rotate_to(ic.mouse[0], 0.5f, pc);

        // cancel gravity
        mc.accel.y -= c_gravity;
        // slow caster down
        mc.accel -= w_normalize(mc.velocity) * w_magnitude(mc.velocity) * 0.9f;
    }

    void move_channel(GameWorld &world, unsigned int entity) {
        auto &pc = world.pos_c(entity);
        auto &mc = world.move_c(entity);
        auto &ic = world.input_c(entity);

        float lounge_speed = 1200;
        float lounge_accel = 4000;
        float change_angle = 0.02;

        auto vel = w_normalize(mc.velocity);
        // normal accel
        mc.accel = w_rotated(WVec(0, -1), pc.rotation * pc.direction) * lounge_accel;

        auto dir = w_normalize(ic.mouse[0] - pc.global_position);
        // when target before actor, steer towards
        if (dot(dir, w_rotated(WVec(0, -1), pc.rotation * pc.direction)) > 0) {
            auto angle =  w_angle_to_vec(w_rotated(WVec(0, -1), pc.rotation * pc.direction), dir);
            rotate_angle(angle * pc.direction, change_angle, pc);

            // steer
            mc.accel = dir * lounge_accel;
        }

        mc.velocity = fmin(w_magnitude(mc.velocity), lounge_speed) * vel;

    }
}

lounge_skill::Skill::Skill() : SkillBase(0.8, 1.2, 0, 5, SkillID::Lounge) {}
