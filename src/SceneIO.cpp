//
// Created by jammer on 18/05/17.
//

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

scene::Entity * get_pb_entity(GameWorld &game_world, unsigned int entity) {
    using namespace std;

    scene::Entity *pb_entity = new scene::Entity;

    // bitset
    auto bset = new scene::Bitset;
    bset->set_bitset(game_world.entities().at(entity).to_ulong());
    pb_entity->set_allocated_bitset(bset);

    // pos_c
    if (game_world.entities().at(entity).test(CPosition)) {
        auto pos_c = new scene::PosComponent;
        auto pos = new scene::Point;
        pos->set_x(game_world.pos_c(entity).position.x);
        pos->set_y(game_world.pos_c(entity).position.y);
        pos_c->set_allocated_position(pos);
        pos_c->set_parent(game_world.pos_c(entity).parent);
        pos_c->set_rotation(game_world.pos_c(entity).rotation);
        pb_entity->set_allocated_pos_c(pos_c);
    }

    // input_c
    if (game_world.entities().at(entity).test(CInput)) {
        auto input_c = new scene::InputComponent;
        input_c->set_func(static_cast<int>(game_world.input_c(entity).input_func));
        pb_entity->set_allocated_input_c(input_c);
    }

    // move_c
    if (game_world.entities().at(entity).test(CMove)) {
        auto move_c = new scene::MoveComponent;
        move_c->set_moveset(static_cast<int>(game_world.move_c(entity).moveset));
        move_c->set_movestate(static_cast<int>(game_world.move_c(entity).movestate));
        move_c->set_mass(game_world.move_c(entity).mass);
        pb_entity->set_allocated_move_c(move_c);
    }

    // static_col_comp
    if (game_world.entities().at(entity).test(CStaticCol)) {
        auto static_c = new scene::StaticColComponent;
        auto capsule = dynamic_cast<ColCapsule *>(game_world.static_col_c(entity).shape.get());
        static_c->set_col_response(static_cast<int>(game_world.static_col_c(entity).col_response));
        static_c->set_shape(static_cast<int>(capsule->m_type));
        static_c->set_length(capsule->m_length);
        static_c->set_radius(capsule->get_capsule_radius());
        pb_entity->set_allocated_static_col_c(static_c);
    }

    // name
    pb_entity->set_name(game_world.name_c(entity));

    // ground move
    if (game_world.entities().at(entity).test(CGroundMove)) {
        auto ground_c = new scene::GroundMoveComponent;
        ground_c->set_accel(game_world.ground_move_c(entity).c_accel);
        ground_c->set_stop_friction(game_world.ground_move_c(entity).c_stop_friction);
        ground_c->set_turn_mod(game_world.ground_move_c(entity).c_turn_mod);
        ground_c->set_max_vel(game_world.ground_move_c(entity).c_max_vel);
        pb_entity->set_allocated_ground_move_c(ground_c);
    }

    // jump
    if (game_world.entities().at(entity).test(CJump)) {
        scene::JumpComponent *jump_c = new scene::JumpComponent;
        jump_c->set_turn_mod(game_world.jump_c(entity).c_turn_mod);
        jump_c->set_accel(game_world.jump_c(entity).c_accel);
        jump_c->set_jump_height(game_world.jump_c(entity).c_jump_height);
        jump_c->set_max_vel(game_world.jump_c(entity).c_max_vel);
        pb_entity->set_allocated_jump_c(jump_c);
    }

    // fly
    if (game_world.entities().at(entity).test(CFly)) {
        auto fly_c = new scene::FlyComponent;
        auto from = new scene::Point;
        auto ctrl_from = new scene::Point;
        scene::Point *ctrl_to = new scene::Point;
        scene::Point *to = new scene::Point;
        fly_c->set_accel_force(game_world.fly_c(entity).c_accel_force);
        fly_c->set_accel_time(game_world.fly_c(entity).c_accel_time);
        fly_c->set_lift(game_world.fly_c(entity).c_lift);
        fly_c->set_max_change_angle(game_world.fly_c(entity).c_max_change_angle);
        fly_c->set_stall_angle(game_world.fly_c(entity).c_stall_angle);
        fly_c->set_push_vel(game_world.fly_c(entity).c_push_vel);
        from->set_x(game_world.fly_c(entity).from.x);
        from->set_y(game_world.fly_c(entity).from.y);
        ctrl_from->set_x(game_world.fly_c(entity).ctrl_from.x);
        ctrl_from->set_y(game_world.fly_c(entity).ctrl_from.y);
        ctrl_to->set_x(game_world.fly_c(entity).ctrl_to.x);
        ctrl_to->set_y(game_world.fly_c(entity).ctrl_to.y);
        to->set_x(game_world.fly_c(entity).to.x);
        to->set_y(game_world.fly_c(entity).to.y);
        fly_c->set_allocated_from(from);
        fly_c->set_allocated_ctrl_from(ctrl_from);
        fly_c->set_allocated_ctrl_to(ctrl_to);
        fly_c->set_allocated_to(to);
        pb_entity->set_allocated_fly_c(fly_c);
    }

    // simple fly
    if (game_world.entities().at(entity).test(CSimpleFly)) {
        auto fly_c = new scene::SimplyFlyComponent;
        fly_c->set_max_vel(game_world.simple_fly_c(entity).c_max_vel);
        fly_c->set_accel(game_world.simple_fly_c(entity).c_accel);
        fly_c->set_near_threshold(game_world.simple_fly_c(entity).c_near_threshold);
        fly_c->set_stop_coef(game_world.simple_fly_c(entity).c_stop_coef);
        pb_entity->set_allocated_simple_fly_c(fly_c);
    }

    return move(pb_entity);
}

bool save_entity_standalone(GameWorld &game_world, unsigned int entity) {
    using namespace std;

    auto pb_entity = get_pb_entity(game_world, entity);
    pb_entity->mutable_pos_c()->mutable_position()->set_x(0);
    pb_entity->mutable_pos_c()->mutable_position()->set_x(0);
    pb_entity->mutable_pos_c()->set_parent(0);

    string entity_name = game_world.name_c(entity);
    string filename = "scenes/" + entity_name + ".went";
    fstream  scene_file(filename, ios_base::out | ios_base::trunc | ios_base::binary);
    if (!pb_entity->SerializeToOstream(&scene_file)) {
        cout << "Failed to write scene." << endl;
        delete pb_entity;
        return false;
    }
    cout << "wrote to file " << filename << endl;
    delete pb_entity;
    return true;
}

void entity_to_world(const scene::Entity &pb_entity, GameWorld &game_world, unsigned int entity) {
// bitset
    auto bitset = pb_entity.bitset();
    game_world.entities()[entity] = bset(bitset.bitset());

    // pos_c
    if (pb_entity.has_pos_c()) {
        auto pos_c = pb_entity.pos_c();
        game_world.pos_c(entity).rotation = pos_c.rotation();
        game_world.pos_c(entity).position = WVec(pos_c.position().x(), pos_c.position().y());
        game_world.pos_c(entity).parent = pos_c.parent();
        game_world.pos_c(entity).global_transform = glm::rotate(glm::translate(game_world.pos_c(pos_c.parent()).
                global_transform, game_world.pos_c(entity).position), pos_c.rotation());
    }

    // input_c
    if (pb_entity.has_input_c()) {
        auto input_c = pb_entity.input_c();
        game_world.input_c(entity).input_func = static_cast<InputFunc>(input_c.func());
    }

    // move_c
    if (pb_entity.has_move_c()) {
        auto move_c = pb_entity.move_c();
        game_world.move_c(entity).movestate = static_cast<MoveState>(move_c.movestate());
        game_world.move_c(entity).moveset = static_cast<MoveSet>(move_c.moveset());
        game_world.move_c(entity).mass = move_c.mass();
    }

    // static_col_c
    if (pb_entity.has_static_col_c()) {
        auto static_c = pb_entity.static_col_c();
        if (static_cast<ColShapeName>(static_c.shape()) != ColShapeName::ColCapsule) {
            assert(false);
        }
        game_world.static_col_c(entity).shape = std::make_shared<ColCapsule>(ColCapsule{static_c.radius(), static_c.length()});
        game_world.static_col_c(entity).col_response = static_cast<StaticColResponse>(static_c.col_response());

        // debug_c
        game_world.debug_c(entity).shape = std::make_shared<ColCapsule>(ColCapsule{static_c.radius(), static_c.length()});
    }


    // name
    game_world.name_c(entity) = pb_entity.name();

    // ground move
    if (pb_entity.has_ground_move_c()) {
        auto ground_move_c = pb_entity.ground_move_c();
        game_world.ground_move_c(entity).c_accel = ground_move_c.accel();
        game_world.ground_move_c(entity).c_turn_mod = ground_move_c.turn_mod();
        game_world.ground_move_c(entity).c_stop_friction = ground_move_c.stop_friction();
        game_world.ground_move_c(entity).c_max_vel = ground_move_c.max_vel();
    }

    // jump move
    if (pb_entity.has_jump_c()) {
        auto jump_c = pb_entity.jump_c();
        game_world.jump_c(entity).c_accel = jump_c.accel();
        game_world.jump_c(entity).c_turn_mod = jump_c.turn_mod();
        game_world.jump_c(entity).c_jump_height = jump_c.jump_height();
        game_world.jump_c(entity).c_max_vel = jump_c.max_vel();
    }

    // fly move
    if (pb_entity.has_fly_c()) {
        auto fly_c = pb_entity.fly_c();
        game_world.fly_c(entity).c_lift = fly_c.lift();
        game_world.fly_c(entity).c_stall_angle = fly_c.stall_angle();
        game_world.fly_c(entity).c_max_change_angle = fly_c.max_change_angle();
        game_world.fly_c(entity).c_accel_time = fly_c.accel_time();
        game_world.fly_c(entity).c_accel_force = fly_c.accel_force();
        game_world.fly_c(entity).c_push_vel = fly_c.push_vel();

        game_world.fly_c(entity).from = WVec(fly_c.from().x(), fly_c.from().y());
        game_world.fly_c(entity).ctrl_from = WVec(fly_c.ctrl_from().x(), fly_c.ctrl_from().y());
        game_world.fly_c(entity).ctrl_to = WVec(fly_c.ctrl_to().x(), fly_c.ctrl_to().y());
        game_world.fly_c(entity).to = WVec(fly_c.to().x(), fly_c.to().y());
    }

    // simply fly
    if (pb_entity.has_simple_fly_c()) {
        auto fly_c = pb_entity.simple_fly_c();
        game_world.simple_fly_c(entity).c_max_vel = fly_c.max_vel();
        game_world.simple_fly_c(entity).c_accel = fly_c.accel();
        game_world.simple_fly_c(entity).c_near_threshold = fly_c.near_threshold();
        game_world.simple_fly_c(entity).c_stop_coef = fly_c.stop_coef();
    }
}

bool load_entity_from_filename(const std::__cxx11::string &name, GameWorld &game_world, unsigned int entity) {
    using namespace std;

    fstream scene_file(name, ios_base::in | ios_base::binary);
    scene::Entity pb_entity;

    if (!scene_file) {
        cout << "Entity file not found: " << name << endl;
        return false;
    }
    pb_entity.ParseFromIstream(&scene_file);

    entity_to_world(pb_entity, game_world, entity);

    return true;
}

void delete_entity_from_scene(GameWorld &game_world, unsigned int entity) {
    game_world.delete_entity_raw(entity);
}

void scene_entity_to_world(const scene::Entity &pb_entity, GameWorld &game_world, unsigned int entity) {
    std::string filename = "scenes/" + pb_entity.name() + ".went";
    if (!(pb_entity.name() == "root") and load_entity_from_filename(filename, game_world, entity)) {
        if (pb_entity.has_pos_c()) {
            auto pos_c = pb_entity.pos_c();
            game_world.pos_c(entity).rotation = pos_c.rotation();
            game_world.pos_c(entity).position = WVec(pos_c.position().x(), pos_c.position().y());
            game_world.pos_c(entity).parent = pos_c.parent();
            game_world.pos_c(entity).global_transform = glm::rotate(glm::translate(game_world.pos_c(pos_c.parent()).
                    global_transform, game_world.pos_c(entity).position), pos_c.rotation());
        }
        return;
    }

    // else we have to load normally from scene
    entity_to_world(pb_entity, game_world, entity);
}
