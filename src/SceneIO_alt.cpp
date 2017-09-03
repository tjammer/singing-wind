#include <fstream>
#include <iostream>
#include <ColShape.h>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_transform_2d.hpp>
#include "GameWorld.h"
#include "Editor.h"
#include "SceneIO.h"
#include "Components.cpp"
#include "InputComponent.h"
#include "CollisionComponent.h"
#include "MoveSystems.h"
#include "PosComponent.h"
#include "SkillComponent.h"
#include "PatrolComponent.h"

#include <flatbuffers/flatbuffers.h>
#include "entity_generated.h"


EntityFBS::EntityT get_fb_entity(GameWorld &world, unsigned int entity) {
    using namespace std;
    using namespace EntityFBS;
    EntityFBS::EntityT fbs_ent;
    const auto &bset = world.entities().at(entity);

    // bitset
    fbs_ent.bitset = bset.to_ulong();

    // poc_c
    if (bset.test(CPosition)) {
        const auto &pos_c = world.pos_c(entity);
        fbs_ent.pos_c = unique_ptr<PosComponentT>(new PosComponentT());
        fbs_ent.pos_c->rotation = pos_c.rotation;
        fbs_ent.pos_c->parent = pos_c.parent;
        fbs_ent.pos_c->position = unique_ptr<Point>(new Point());
        fbs_ent.pos_c->position->mutate_x(pos_c.position.x);
        fbs_ent.pos_c->position->mutate_y(pos_c.position.y);
    }

    // input_c
    if (bset.test(CInput)) {
        fbs_ent.input_c = static_cast<int>(world.input_c(entity).input_func);
    }

    // move_c
    if (bset.test(CMove)) {
        fbs_ent.move_c = unique_ptr<MoveComponentT>(new MoveComponentT);
        fbs_ent.move_c->moveset = static_cast<int>(world.move_c(entity).moveset);
        fbs_ent.move_c->movestate = static_cast<int>(world.move_c(entity).movestate);
        fbs_ent.move_c->mass = world.move_c(entity).mass;
    }

    // shape_c
    if (bset.test(CColShape)) {
        const auto &capsule = dynamic_cast<ColCapsule *>(world.cshape_c(entity).shape.get());
        fbs_ent.shape_c = unique_ptr<ShapeComponentT>(new ShapeComponentT);
        fbs_ent.shape_c->shape = static_cast<int>(capsule->m_type);
        fbs_ent.shape_c->length = capsule->m_length;
        fbs_ent.shape_c->radius = capsule->get_capsule_radius();
    }

    // static col
    if (bset.test(CStaticCol)) {
        fbs_ent.static_col_c = static_cast<int>(world.static_col_c(entity).col_response);
    }

    // name
    fbs_ent.name = world.name_c(entity);

    // gound_move_c
    if (bset.test(CGroundMove)) {
        const auto &gc = world.ground_move_c(entity);
        fbs_ent.ground_move_c = unique_ptr<GroundMoveComponentT>(new GroundMoveComponentT);
        fbs_ent.ground_move_c->accel = gc.c_accel;
        fbs_ent.ground_move_c->turn_mod = gc.c_turn_mod;
        fbs_ent.ground_move_c->stop_friction = gc.c_stop_friction;
        fbs_ent.ground_move_c->max_vel = gc.c_max_vel;
    }

    // fall_c
    if (bset.test(CFall)) {
        const auto &fc = world.fall_c(entity);
        fbs_ent.fall_c = unique_ptr<FallComponentT>(new FallComponentT);
        fbs_ent.fall_c->accel = fc.c_accel;
        fbs_ent.fall_c->turn_mod = fc.c_turn_mod;
        fbs_ent.fall_c->jump_height = fc.c_jump_height;
        fbs_ent.fall_c->max_vel = fc.c_max_vel;
    }

    // fly_c
    if (bset.test(CFly)) {
        const auto &fc = world.fly_c(entity);
        fbs_ent.fly_c = unique_ptr<FlyComponentT>(new FlyComponentT);
        auto &fbs_fc = fbs_ent.fly_c;
        fbs_fc->lift = fc.c_lift;
        fbs_fc->stall_angle = fc.c_stall_angle;
        fbs_fc->max_change_angle = fc.c_max_change_angle;
        fbs_fc->accel_force = fc.c_accel_force;
        fbs_fc->accel_time = fc.c_accel_time;
        fbs_fc->push_vel = fc.c_push_vel;

        fbs_fc->to = unique_ptr<Point>(new Point);
        fbs_fc->ctrl_to = unique_ptr<Point>(new Point);
        fbs_fc->ctrl_from = unique_ptr<Point>(new Point);
        fbs_fc->from = unique_ptr<Point>(new Point);
        fbs_fc->to->mutate_x(fc.to.x);
        fbs_fc->to->mutate_y(fc.to.y);
        fbs_fc->ctrl_to->mutate_x(fc.ctrl_to.x);
        fbs_fc->ctrl_to->mutate_y(fc.ctrl_to.y);
        fbs_fc->ctrl_from->mutate_x(fc.ctrl_from.x);
        fbs_fc->ctrl_from->mutate_y(fc.ctrl_from.y);
        fbs_fc->from->mutate_x(fc.from.x);
        fbs_fc->from->mutate_y(fc.from.y);
    }

    // simple fly
    if (bset.test(CSimpleFly)) {
        const auto &fc = world.simple_fly_c(entity);
        fbs_ent.simple_fly_c = unique_ptr<SimpleFlyComponentT>(new SimpleFlyComponentT);
        auto &fbc_fc = fbs_ent.simple_fly_c;

        fbc_fc->max_vel = fc.c_max_vel;
        fbc_fc->accel = fc.c_accel;
        fbc_fc->near_threshold = fc.c_near_threshold;
        fbc_fc->stop_coef = fc.c_stop_coef;
    }

    // dyn_col_c
    if (bset.test(CDynCol)) {
        fbs_ent.dyn_col_c = static_cast<int>(world.dyn_col_c(entity).col_response);
    }

    // tag
    if (bset.test(CTag)) {
        fbs_ent.tag_c = world.tag_c(entity).to_ulong();
    }

    // skill_c
    if (bset.test(CSkill)) {
        auto &skill_vec = fbs_ent.skill_c;
        skill_vec.clear();

        for (const auto &skill : world.skill_c(entity).skills) {
            skill_vec.push_back(static_cast<int>(skill->id));
        }
    }

    // patrol_c
    if (bset.test(CPatrol)) {
        fbs_ent.patrol_c = unique_ptr<PatrolComponentT>(new PatrolComponentT);
        fbs_ent.patrol_c->pp = unique_ptr<Point>(new Point);
        fbs_ent.patrol_c->pp->mutate_x(world.patrol_c(entity).patrol_point.x);
        fbs_ent.patrol_c->pp->mutate_y(world.patrol_c(entity).patrol_point.y);
    }

    return fbs_ent;
}

bool save_entity_standalone_fbs(GameWorld &world, unsigned int entity) {
    using namespace std;

    auto fbs_ent = get_fb_entity(world, entity);
    flatbuffers::FlatBufferBuilder fbb;
    fbb.Finish(EntityFBS::Entity::Pack(fbb, &fbs_ent));

    string entity_name = world.name_c(entity);
    string filename = "scenes/" + entity_name + ".wentfbs";
    fstream scene_file(filename, ios_base::out | ios_base::binary);
    scene_file.write(reinterpret_cast<char *>(fbb.GetBufferPointer()), fbb.GetSize());
    scene_file.close();

    return true;
}
