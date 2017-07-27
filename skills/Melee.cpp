#include "Melee.h"
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

#include <algorithm>

void melee_skill_hurtfunc(GameWorld &world, unsigned int entity) {
    const auto &dc = world.dyn_col_c(entity);
    auto &hb = world.hurtbox_c(entity);
    entity = world.hurtbox_c(entity).owner;
    if (entity == dc.collided) {
        return;
    }
    if (std::find(hb.hit_entities.begin(), hb.hit_entities.end(), dc.collided) != hb.hit_entities.end()) {
        return ;
    }
    // knockback
    auto dir = w_normalize(world.pos_c(dc.collided).position - world.pos_c(entity).position);
    world.move_c(dc.collided).velocity = dir * 300.f;
    auto kb = statuseffect_knockback();
    kb.timer = .5f;
    add_effect(world, dc.collided, kb);
    // TODO: damage
    hb.hit_entities.push_back(dc.collided);
}

void melee_skill::on_channel(GameWorld &world, unsigned int entity) {
    auto &sc = world.skill_c(entity);
    auto &skill = sc.skills.at(sc.active);
    
    if (skill.timer != skill.c_time_channel) {
        return;
    }
    // create hurtbox
    auto hurtbox = world.create_entity();
    bset comps;
    for (auto i : {CPosition, CColShape, CDynCol, CDebugDraw, CTag, CLifeTime}) {
        comps.set(i);
    }
    
    world.entities()[hurtbox] = comps;
    world.name_c(hurtbox) = "melee_skill_hurtbox";

    // pos
    auto &pc = world.pos_c(hurtbox);
    pc.parent = entity;
    // figure out direction
    const auto &parent_ic = world.input_c(entity);
    pc.position = WVec(0, -35);
    pc.rotation = 0;
    // col shape
    auto &csc = world.cshape_c(hurtbox);
    csc.shape = std::shared_ptr<ColShape>(new ColCapsule(35, 15));

    // tags
    auto &tc = world.tag_c(hurtbox);
    tc.set(static_cast<int>(Tags::Hurtbox));

    // dyn col
    auto &dc = world.dyn_col_c(hurtbox);
    dc.col_response = DynColResponse::HurtBox;

    // lifetime
    auto &lc = world.lifetime_c(hurtbox);
    lc.timer = .3;

    // hurtbox
    auto &hb = world.hurtbox_c(hurtbox);
    hb.owner = entity;
    hb.hit_function = melee_skill_hurtfunc;
}
