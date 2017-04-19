//
// Created by tobi on 3/28/17.
//

#include "Editor.h"
#include "EditorStates.h"
#include "triangulate.h"
#include <imgui.h>
#include <iostream>
#include <fstream>
#include <Scene.pb.h>

const float zoom_constant = 0.05f;

void EngineEditorState::draw(sf::RenderWindow &window) {
    if (m_paused) {
        return;
    }

    m_state->draw(game_world, window);
}

EngineEditorState::EngineEditorState(const std::string &scene_name, GameWorld &game_world)
        :
        game_world(game_world) {
    m_state = std::unique_ptr<BaseEditorSubState>(new EditorIdle);
    load_scene(scene_name);
}

void EngineEditorState::update(Engine &engine) {
    if (!engine.get_focus()) {
        return;
    }
    const sf::RenderWindow& window = engine.get_window();
    auto imouse = sf::Mouse::getPosition(window);
    auto mouse = window.mapPixelToCoords(imouse);

    if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right) and not m_pressed[Menu]) {
        m_menu = !m_menu;
        ImGui::SetNextWindowPos(imouse);
    }
    m_pressed[Menu] = sf::Mouse::isButtonPressed(sf::Mouse::Button::Right);

    if (m_menu) {
        auto transition = m_state->menu(game_world);
        if (transition != nullptr) {
            m_state = std::move(transition);
            m_menu = false;
            update_world();
        }
        else {
            if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) and not m_pressed[Confirm]) {
                if (!ImGui::IsAnyItemHovered()) {
                    m_menu = false;
                }
            }
            m_pressed[Confirm] = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
        }
        if (menu()) {
            m_menu = false;
        }
        return;
    }

    m_state->update(mouse);

    if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) and not m_pressed[Confirm]) {
        m_state = m_state->confirm(game_world);
        update_world();
    }
    m_pressed[Confirm] = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape) and not m_pressed[Cancel]) {
        m_state = m_state->cancel();
    }
    m_pressed[Cancel] = sf::Keyboard::isKeyPressed(sf::Keyboard::Escape);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::G) and not m_pressed[Move]) {
        m_state = m_state->move(game_world);
    }
    m_pressed[Move] = sf::Keyboard::isKeyPressed(sf::Keyboard::G);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::I) and not m_pressed[Insert]) {
        m_state = m_state->insert_item(game_world);
        update_world();
    }
    m_pressed[Insert] = sf::Keyboard::isKeyPressed(sf::Keyboard::I);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) and not m_pressed[Delete]) {
        m_state = m_state->delete_item(game_world);
        update_world();
    }
    m_pressed[Delete] = sf::Keyboard::isKeyPressed(sf::Keyboard::D);


    auto idiff = imouse - m_mouse;
    m_mouse = imouse;
    if (sf::Mouse::isButtonPressed(sf::Mouse::Middle)) {
        auto diff = WVec(idiff.x, idiff.y);
        auto view = window.getView();
        view.move(-diff * m_zoom);
        engine.set_view(view);
    }

    if (engine.get_mouse_wheel() != m_mouse_wheel) {
        m_zoom -= zoom_constant * (engine.get_mouse_wheel() - m_mouse_wheel);
        m_mouse_wheel = engine.get_mouse_wheel();
        m_update_view = true;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Home) and not m_pressed[ResetZoom]) {
        m_zoom = 1.0f;
        m_mouse_wheel = engine.get_mouse_wheel();
        m_update_view = true;
    }
    m_pressed[ResetZoom] = sf::Keyboard::isKeyPressed(sf::Keyboard::Home);

    if (m_update_view) {
        m_update_view = false;

        auto view = window.getView();
        auto default_size = window.getDefaultView().getSize();
        view.setSize(default_size.x * m_zoom, default_size.y * m_zoom);
        engine.set_view(view);
    }

}

void EngineEditorState::update_world() {
    game_world.update_triangles();
}

bool EngineEditorState::load_scene(const std::string &name) {
    using namespace std;
    auto &islands = game_world.get_islands_ref();

    fstream scene_file(name, ios::in | ios::binary);
    scene::Scene pb_scene;

    if (!scene_file) {
        cout << "File not found." << endl;
        return false;
    }
    pb_scene.ParseFromIstream(&scene_file);
    islands.clear();
    for (auto &pb_island : pb_scene.islands()) {
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
    m_zoom = pb_scene.zoom();
    m_update_view = true;

    update_world();
    return true;
}

void EngineEditorState::save_scene(const std::string &name) {
    using namespace std;
    auto &islands = game_world.get_islands_ref();

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

    fstream  scene_file(name, ios::out | ios::trunc | ios::binary);
    if (!pb_scene.SerializeToOstream(&scene_file)) {
        cout << "Failed to write scene." << endl;
    }
    cout << "wrote to file " << name << endl;
}

bool EngineEditorState::menu() {
    bool rtn = false;
    ImGui::BeginMainMenuBar();
    if (ImGui::BeginMenu("scene")) {
        if (ImGui::MenuItem("save scene")) {
            save_scene("debug.wscn");
            rtn = true;
        }
        ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();
    return rtn;
}

