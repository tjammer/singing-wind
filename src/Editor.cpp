//
// Created by tobi on 3/28/17.
//

#include "Editor.h"
#include "EditorStates.h"
#include "ColShape.h"
#include "GameWorld.h"
#include "SceneIO.h"
#include "WRenderer.h"
#include <imgui.h>
#include <iostream>
#include <fstream>
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
    m_game_world.update_world();
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
        scene_entity_to_world(pb_entity, m_game_world, m_game_world.create_entity());
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
        if (ImGui::BeginMenu("view")) {
            rtn = true;
            if (ImGui::MenuItem("NavMesh / visibility graph")) {
                m_state = EditorSubState(new NavMeshIdle());
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

void EngineEditorState::unpause() {
    EngineState::unpause();
    auto current = glm::inverse(WRenderer::get_camera());
    WVec center = {current[3][0], current[3][1]};
    m_camera.set_center(center);
    m_camera.update({0, 0}, m_zoom);
}

