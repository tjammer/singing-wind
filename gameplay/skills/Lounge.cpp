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

void LoungeAttackMove::enter(GameWorld &world, unsigned int entity) {
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
    lc.timer = TimedMoveState::timer;

    // hurtbox
    auto &hb = world.hurtbox_c(hurtbox);
    hb.owner = entity;
    hb.hit_entities.clear();
    hb.hurt_function = lounge_skill_hurtfunc;
    hb.on_hit = lounge_skill_on_hit;
}

void LoungeCastMove::accel(GameWorld &world, unsigned int entity) {
    auto &pc = world.pos_c(entity);
    auto &ic = world.input_c(entity);
    auto &mc = world.move_c(entity);

    rotate_to(ic.mouse.get(), mc.c_max_change_angle * 1.5f, pc);

    // cancel gravity
    mc.accel.y -= c_gravity;
    // slow caster down
    mc.accel -= w_normalize(mc.velocity) * w_magnitude(mc.velocity) * 0.9f;
}

void LoungeAttackMove::accel(GameWorld &world, unsigned int entity) {
    auto &pc = world.pos_c(entity);
    auto &mc = world.move_c(entity);
    auto &ic = world.input_c(entity);

    float lounge_speed = 1200;
    float lounge_accel = 3500;
    float change_angle = mc.c_max_change_angle * 0.5f;

    auto vel = w_normalize(mc.velocity);
    // normal accel
    mc.accel = w_rotated(WVec(0, -1), pc.rotation * pc.direction) * lounge_accel;

    auto dir = w_normalize(ic.mouse.get() - pc.global_position);
    // when target before actor, steer towards
    if (dot(dir, w_rotated(WVec(0, -1), pc.rotation * pc.direction)) > 0) {
        auto angle =  w_angle_to_vec(w_rotated(WVec(0, -1), pc.rotation * pc.direction), dir);
        rotate_angle(angle * pc.direction, change_angle, pc);

        // steer
        mc.accel = dir * lounge_accel;
    }

    mc.velocity = fmin(w_magnitude(mc.velocity), lounge_speed) * vel;

}

void LoungeSkill::set_special(GameWorld &world, unsigned int entity) {
    world.move_c(entity).special_movestate = std::make_unique<LoungeCastMove>();
}
