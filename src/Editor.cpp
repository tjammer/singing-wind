//
// Created by tobi on 3/28/17.
//

#include "Editor.h"
#include "EditorStates.h"
#include "ColShape.h"
#include "GameWorld.h"
#include <imgui.h>
#include <iostream>
#include <fstream>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_transform_2d.hpp>
#include <GLFW/glfw3.h>

const float zoom_constant = 0.05f;

void EngineEditorState::draw() {
    if (m_paused) {
        return;
    }
    m_state->draw(m_game_world);
}

EngineEditorState::EngineEditorState(const std::string &scene_name, GameWorld &game_world)
        : m_game_world(game_world) {
    m_state = std::unique_ptr<BaseEditorSubState>(new EditorIdle);
    load_scene(scene_name);
    m_camera.update({0, 0}, m_zoom);
}

void EngineEditorState::update(Engine &engine) {
    if (!engine.get_focus()) {
        return;
    }
    GLFWwindow& window = engine.get_window();
    double mpos[2];
    glfwGetCursorPos(&window, &mpos[0], &mpos[1]);
    WVec mouse = m_camera.unproject_mouse(mpos);
    if (main_menu()) {
        return;
    }

    if (glfwGetMouseButton(&window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS and not m_pressed[Menu]) {
        m_menu = !m_menu;
        ImGui::SetNextWindowPos({(float)mpos[0], (float)mpos[1]});
    }
    m_pressed[Menu] = glfwGetMouseButton(&window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;

    if (m_menu) {
        auto transition = m_state->menu(m_game_world);
        if (transition != nullptr) {
            m_state = std::move(transition);
            m_menu = false;
            update_world();
        }
        else {
            if (glfwGetMouseButton(&window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS and not m_pressed[Confirm]) {
                if (!ImGui::IsAnyItemHovered()) {
                    m_menu = false;
                }
            }
            m_pressed[Confirm] = glfwGetMouseButton(&window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
        }
        return;
    }

    auto transition = m_state->update(mouse);
    if (transition != nullptr) {
        m_state = std::move(transition);
        update_world();
    }

    if (glfwGetMouseButton(&window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS and not m_pressed[Confirm]) {
        transition = m_state->confirm(m_game_world);
        if (transition != nullptr) {
            m_state = std::move(transition);
            update_world();
        }
    }
    m_pressed[Confirm] = glfwGetMouseButton(&window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;

    if (glfwGetKey(&window, GLFW_KEY_ESCAPE) == GLFW_PRESS and not m_pressed[Cancel]) {
        transition = m_state->cancel();
        if (transition != nullptr) {
            m_state = std::move(transition);
            update_world();
        }
    }
    m_pressed[Cancel] = glfwGetKey(&window, GLFW_KEY_ESCAPE) == GLFW_PRESS;

    if (glfwGetKey(&window, GLFW_KEY_G) == GLFW_PRESS and not m_pressed[Move]) {
        transition = m_state->move(m_game_world);
        if (transition != nullptr) {
            m_state = std::move(transition);
            update_world();
        }
    }
    m_pressed[Move] = glfwGetKey(&window, GLFW_KEY_G) == GLFW_PRESS;

    if (glfwGetKey(&window, GLFW_KEY_I) == GLFW_PRESS and not m_pressed[Insert]) {
        transition = m_state->insert_item(m_game_world);
        if (transition != nullptr) {
            m_state = std::move(transition);
            update_world();
        }
    }
    m_pressed[Insert] = glfwGetKey(&window, GLFW_KEY_I) == GLFW_PRESS;

    if (glfwGetKey(&window, GLFW_KEY_D) == GLFW_PRESS and not m_pressed[Delete]) {
        transition = m_state->delete_item(m_game_world);
        if (transition != nullptr) {
            m_state = std::move(transition);
            update_world();
        }
    }
    m_pressed[Delete] = glfwGetKey(&window, GLFW_KEY_D) == GLFW_PRESS;

    auto screen_mouse = WVec(mpos[0],mpos[1]);
    auto idiff = m_mouse - screen_mouse;
    m_mouse = screen_mouse;
    if (glfwGetMouseButton(&window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS) {
        m_camera.update(m_zoom * idiff, m_zoom);
    }

    if (engine.get_mouse_wheel() != m_mouse_wheel) {
        m_zoom -= zoom_constant * (engine.get_mouse_wheel() - m_mouse_wheel);
        m_mouse_wheel = engine.get_mouse_wheel();
    }

    if (glfwGetKey(&window, GLFW_KEY_HOME) == GLFW_PRESS and not m_pressed[ResetZoom]) {
        m_zoom = 1.0f;
        m_mouse_wheel = engine.get_mouse_wheel();
    }
    m_pressed[ResetZoom] = glfwGetKey(&window, GLFW_KEY_HOME) == GLFW_PRESS;

}

void EngineEditorState::update_world() {
    m_game_world.update_triangles();
}

bool EngineEditorState::load_scene(const std::string &name) {
    using namespace std;
    auto &islands = m_game_world.get_islands_ref();

    string filename = "scenes/" + name + ".wscn";
    fstream scene_file(filename, ios::in | ios::binary);
    scene::Scene pb_scene;
    if (!scene_file) {
        cout << "File not found." << filename << endl;
        return false;
    }
    pb_scene.ParseFromIstream(&scene_file);
    islands.clear();
    for (auto pb_island : pb_scene.islands()) {
        Island island;
        for (int i = 0 ; i < pb_island.points_size() ; ++i) {
            auto pb_point = pb_island.points(i);
            island.m_points.push_back(WVec(pb_point.x(), pb_point.y()));
        }
        for (int i = 0 ; i < pb_island.ctrl_points_size() ; ++i) {
            auto pb_point = pb_island.ctrl_points(i);
            island.m_ctrl_points.push_back(WVec(pb_point.x(), pb_point.y()));
        }
        islands.push_back(island);
    }
    m_game_world.reset_entities();
    for (auto pb_entity : pb_scene.entities()) {
        entity_to_world(pb_entity, m_game_world, m_game_world.create_entity());
    }

    m_zoom = pb_scene.zoom();

    update_world();
    return true;
}

void EngineEditorState::save_scene(const std::string &name) const {
    using namespace std;
    const auto &islands = m_game_world.get_islands_ref();
    const auto &entities = m_game_world.m_entities;

    scene::Scene pb_scene;
    for (const auto &island : islands) {
        auto pb_island = pb_scene.add_islands();
        for (const auto &point : island.m_points) {
            auto pb_point = pb_island->add_points();
            pb_point->set_x(point.x);
            pb_point->set_y(point.y);
        }
        for (const auto &point : island.m_ctrl_points) {
            auto pb_point = pb_island->add_ctrl_points();
            pb_point->set_x(point.x);
            pb_point->set_y(point.y);
        }
    }
    pb_scene.set_zoom(m_zoom);

    for (unsigned int i = 0 ; i < entities.size() ; ++i) {
        auto pb_ent = pb_scene.add_entities();
        scene::Entity *standalone_ent = get_pb_entity(m_game_world, i);
        memcpy(pb_ent, standalone_ent, sizeof(*standalone_ent));
    }

    string filename = "scenes/" + name + ".wscn";
    fstream  scene_file(filename, ios::out | ios::trunc | ios::binary);
    if (!pb_scene.SerializeToOstream(&scene_file)) {
        cout << "Failed to write scene." << endl;
    }
    cout << "wrote to file " << filename << endl;
}

inline void new_scene(GameWorld& world) {
    world.reset_entities();
    world.reset_islands();
    world.create_root();
}

bool EngineEditorState::main_menu() {
    bool rtn = false;
    bool load_ent = false;
    bool load_scn = false;
    bool save = false;
    static std::string ent_name = "";

    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("scene")) {
            rtn = true;
            if (ImGui::MenuItem("new scene")) {
                new_scene(m_game_world);
                rtn = true;
            }
            if (ImGui::MenuItem("save scene")) {
                save = true;
                rtn = true;
            }
            if (ImGui::MenuItem("load entity")) {
                load_ent = true;
                rtn = true;
            }
            if (ImGui::MenuItem("load scene")) {
                load_scn = true;
                rtn = true;
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    if (load_ent) {
        ImGui::OpenPopup("load entity");
    }
    if (ImGui::BeginPopup("load entity")) {
        std::vector<char> entity_name(ent_name.begin(), ent_name.end());
        entity_name.push_back('\0');
        entity_name.resize(128);
        ImGui::InputText("entity name", &entity_name[0], entity_name.size());
        ent_name = std::string(&entity_name[0]);
        if (ImGui::Button("load entity")) {
            m_game_world.load_entity(ent_name);
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
        rtn = true;
    }

    if (save) {
        ImGui::OpenPopup("save scene");
    }
    if (ImGui::BeginPopup("save scene")) {
        std::vector<char> scene_name(ent_name.begin(), ent_name.end());
        scene_name.push_back('\0');
        scene_name.resize(128);
        ImGui::InputText("scene name", &scene_name[0], scene_name.size());
        ent_name = std::string(&scene_name[0]);
        if (ImGui::Button("save scene")) {
            save_scene(ent_name);
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
        rtn = true;
    }

    if (load_scn) {
        ImGui::OpenPopup("load scene");
    }
    if (ImGui::BeginPopup("load scene")) {
        std::vector<char> scene_name(ent_name.begin(), ent_name.end());
        scene_name.push_back('\0');
        scene_name.resize(128);
        ImGui::InputText("scene name", &scene_name[0], scene_name.size());
        ent_name = std::string(&scene_name[0]);
        if (ImGui::Button("load scene")) {
            load_scene(ent_name);
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
        rtn = true;
    }

    return rtn;
}

scene::Entity * get_pb_entity(const GameWorld &game_world, unsigned int entity) {
    using namespace std;

    scene::Entity *pb_entity = new scene::Entity;

    // bitset
    auto bset = new scene::Bitset;
    bset->set_bitset(game_world.m_entities.at(entity).to_ulong());
    pb_entity->set_allocated_bitset(bset);

    // pos_c
    if (game_world.m_entities.at(entity).test(CPosition)) {
        auto pos_c = new scene::PosComponent;
        auto pos = new scene::Point;
        pos->set_x(game_world.m_pos_c.at(entity).position.x);
        pos->set_y(game_world.m_pos_c.at(entity).position.y);
        pos_c->set_allocated_position(pos);
        pos_c->set_parent(game_world.m_pos_c.at(entity).parent);
        pos_c->set_rotation(game_world.m_pos_c.at(entity).rotation);
        pb_entity->set_allocated_pos_c(pos_c);
    }

    // input_c
    if (game_world.m_entities.at(entity).test(CInput)) {
        auto input_c = new scene::InputComponent;
        input_c->set_func(static_cast<int>(game_world.m_input_c.at(entity).input_func));
        pb_entity->set_allocated_input_c(input_c);
    }

    // move_c
    if (game_world.m_entities.at(entity).test(CMove)) {
        auto move_c = new scene::MoveComponent;
        move_c->set_moveset(static_cast<int>(game_world.m_move_c.at(entity).moveset));
        move_c->set_movestate(static_cast<int>(game_world.m_move_c.at(entity).movestate));
        pb_entity->set_allocated_move_c(move_c);
    }

    // static_col_comp
    if (game_world.m_entities.at(entity).test(CStaticCol)) {
        auto static_c = new scene::StaticColComponent;
        auto capsule = dynamic_cast<ColCapsule *>(game_world.m_static_col_c.at(entity).shape.get());
        static_c->set_col_response(static_cast<int>(game_world.m_static_col_c.at(entity).col_response));
        static_c->set_shape(static_cast<int>(capsule->m_type));
        static_c->set_length(capsule->m_length);
        static_c->set_radius(capsule->get_capsule_radius());
        pb_entity->set_allocated_static_col_c(static_c);
    }

    // name
    pb_entity->set_name(game_world.m_id_c[entity]);

    // ground move
    if (game_world.m_entities.at(entity).test(CGroundMove)) {
        auto ground_c = new scene::GroundMoveComponent;
        ground_c->set_accel(game_world.m_ground_move_c.at(entity).c_accel);
        ground_c->set_stop_friction(game_world.m_ground_move_c.at(entity).c_stop_friction);
        ground_c->set_turn_mod(game_world.m_ground_move_c.at(entity).c_turn_mod);
        pb_entity->set_allocated_ground_move_c(ground_c);
    }

    // jump
    if (game_world.m_entities.at(entity).test(CJump)) {
        scene::JumpComponent *jump_c = new scene::JumpComponent;
        jump_c->set_turn_mod(game_world.m_jump_c.at(entity).c_turn_mod);
        jump_c->set_accel(game_world.m_jump_c.at(entity).c_accel);
        jump_c->set_jump_speed(game_world.m_jump_c.at(entity).c_jump_speed);
        pb_entity->set_allocated_jump_c(jump_c);
    }

    // fly
    if (game_world.m_entities.at(entity).test(CFly)) {
        auto fly_c = new scene::FlyComponent;
        auto from = new scene::Point;
        auto ctrl_from = new scene::Point;
        scene::Point *ctrl_to = new scene::Point;
        scene::Point *to = new scene::Point;
        fly_c->set_accel_force(game_world.m_fly_c.at(entity).c_accel_force);
        fly_c->set_accel_time(game_world.m_fly_c.at(entity).c_accel_time);
        fly_c->set_lift(game_world.m_fly_c.at(entity).c_lift);
        fly_c->set_max_change_angle(game_world.m_fly_c.at(entity).c_max_change_angle);
        fly_c->set_stall_angle(game_world.m_fly_c.at(entity).c_stall_angle);
        from->set_x(game_world.m_fly_c.at(entity).from.x);
        from->set_y(game_world.m_fly_c.at(entity).from.y);
        ctrl_from->set_x(game_world.m_fly_c.at(entity).ctrl_from.x);
        ctrl_from->set_y(game_world.m_fly_c.at(entity).ctrl_from.y);
        ctrl_to->set_x(game_world.m_fly_c.at(entity).ctrl_to.x);
        ctrl_to->set_y(game_world.m_fly_c.at(entity).ctrl_to.y);
        to->set_x(game_world.m_fly_c.at(entity).to.x);
        to->set_y(game_world.m_fly_c.at(entity).to.y);
        fly_c->set_allocated_from(from);
        fly_c->set_allocated_ctrl_from(ctrl_from);
        fly_c->set_allocated_ctrl_to(ctrl_to);
        fly_c->set_allocated_to(to);
        pb_entity->set_allocated_fly_c(fly_c);
    }

    return std::move(pb_entity);
}

bool save_entity_standalone(const GameWorld &game_world, unsigned int entity) {
    using namespace std;

    auto pb_entity = get_pb_entity(game_world, entity);
    pb_entity->mutable_pos_c()->mutable_position()->set_x(0);
    pb_entity->mutable_pos_c()->mutable_position()->set_x(0);
    pb_entity->mutable_pos_c()->set_parent(0);

    string entity_name = game_world.m_id_c[entity];
    string filename = "scenes/" + entity_name + ".went";
    fstream  scene_file(filename, ios::out | ios::trunc | ios::binary);
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
    game_world.m_entities[entity] = bset(bitset.bitset());

    // pos_c
    if (pb_entity.has_pos_c()) {
        auto pos_c = pb_entity.pos_c();
        game_world.m_pos_c[entity].rotation = pos_c.rotation();
        game_world.m_pos_c[entity].position = WVec(pos_c.position().x(), pos_c.position().y());
        game_world.m_pos_c[entity].parent = pos_c.parent();
        game_world.m_pos_c[entity].global_transform = glm::rotate(glm::translate(game_world.m_pos_c[pos_c.parent()].
                global_transform, game_world.m_pos_c[entity].position), pos_c.rotation());
    }

    // input_c
    if (pb_entity.has_input_c()) {
        auto input_c = pb_entity.input_c();
        game_world.m_input_c[entity].input_func = static_cast<InputFunc>(input_c.func());
    }

    // move_c
    if (pb_entity.has_move_c()) {
        auto move_c = pb_entity.move_c();
        game_world.m_move_c[entity].movestate = static_cast<MoveState>(move_c.movestate());
        game_world.m_move_c[entity].moveset = static_cast<MoveSet>(move_c.moveset());
    }

    // static_col_c
    if (pb_entity.has_static_col_c()) {
        auto static_c = pb_entity.static_col_c();
        if (static_cast<ColShapeName>(static_c.shape()) != ColShapeName::ColCapsule) {
            assert(false);
        }
        game_world.m_static_col_c[entity].shape = std::make_shared<ColCapsule>(ColCapsule{static_c.radius(), static_c.length()});
        game_world.m_static_col_c[entity].col_response = static_cast<StaticColResponse>(static_c.col_response());

        // debug_c
        game_world.m_debug_c[entity].shape = std::make_shared<ColCapsule>(ColCapsule{static_c.radius(), static_c.length()});
    }


    // name
    game_world.m_id_c[entity] = pb_entity.name();

    // ground move
    if (pb_entity.has_ground_move_c()) {
        auto ground_move_c = pb_entity.ground_move_c();
        game_world.m_ground_move_c[entity].c_accel = ground_move_c.accel();
        game_world.m_ground_move_c[entity].c_turn_mod = ground_move_c.turn_mod();
        game_world.m_ground_move_c[entity].c_stop_friction = ground_move_c.stop_friction();
    }

    // jump move
    if (pb_entity.has_jump_c()) {
        auto jump_c = pb_entity.jump_c();
        game_world.m_jump_c[entity].c_accel = jump_c.accel();
        game_world.m_jump_c[entity].c_turn_mod = jump_c.turn_mod();
        game_world.m_jump_c[entity].c_jump_speed = jump_c.jump_speed();
    }

    // fly move
    if (pb_entity.has_fly_c()) {
        auto fly_c = pb_entity.fly_c();
        game_world.m_fly_c[entity].c_lift = fly_c.lift();
        game_world.m_fly_c[entity].c_stall_angle = fly_c.stall_angle();
        game_world.m_fly_c[entity].c_max_change_angle = fly_c.max_change_angle();
        game_world.m_fly_c[entity].c_accel_time = fly_c.accel_time();
        game_world.m_fly_c[entity].c_accel_force = fly_c.accel_force();

        game_world.m_fly_c[entity].from = WVec(fly_c.from().x(), fly_c.from().y());
        game_world.m_fly_c[entity].ctrl_from = WVec(fly_c.ctrl_from().x(), fly_c.ctrl_from().y());
        game_world.m_fly_c[entity].ctrl_to = WVec(fly_c.ctrl_to().x(), fly_c.ctrl_to().y());
        game_world.m_fly_c[entity].to = WVec(fly_c.to().x(), fly_c.to().y());
    }
}

bool load_entity_from_filename(const std::string &name, GameWorld &game_world, unsigned int entity) {
    using namespace std;

    fstream scene_file(name, ios::in | ios::binary);
    scene::Entity pb_entity;

    if (!scene_file) {
        cout << "File not found." << endl;
        return false;
    }
    pb_entity.ParseFromIstream(&scene_file);

    entity_to_world(pb_entity, game_world, entity);

    return true;
}

void delete_entity_from_scene(GameWorld &game_world, unsigned int entity) {
    game_world.delete_entity_raw(entity);
}

