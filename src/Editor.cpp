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

void EngineEditorState::draw(sf::RenderWindow &window) {
    if (m_paused) {
        return;
    }
    using namespace sf;

    auto curves = m_island.get_curves(c_line_draw_distance);
    VertexArray va(Quads);
    for (auto& v : curves) {
        va.append(v);
    }

    auto verts = m_island.get_points(c_point_size);
    for (auto& v : verts) {
        va.append(v);
    }

    window.draw(va);
    m_state->draw(window);
}

EngineEditorState::EngineEditorState(const std::string &scene_name, GameWorld& game_world) :
        game_world(game_world), m_island(game_world.get_island_ref()) {
    m_state = std::unique_ptr<BaseEditorSubState>(new IslandIdle);
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
        auto transition = m_state->menu(m_island);
        if (transition != nullptr) {
            m_state = std::move(transition);
            m_menu = false;
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
        m_state = m_state->confirm(m_island);
        update_world();
    }
    m_pressed[Confirm] = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape) and not m_pressed[Cancel]) {
        m_state = m_state->cancel();
    }
    m_pressed[Cancel] = sf::Keyboard::isKeyPressed(sf::Keyboard::Escape);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::G) and not m_pressed[Move]) {
        m_state = m_state->move(m_island);
    }
    m_pressed[Move] = sf::Keyboard::isKeyPressed(sf::Keyboard::G);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::I) and not m_pressed[Insert]) {
        m_state = m_state->insert_item(m_island);
        update_world();
    }
    m_pressed[Insert] = sf::Keyboard::isKeyPressed(sf::Keyboard::I);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) and not m_pressed[Delete]) {
        m_state = m_state->delete_item(m_island);
        update_world();
    }
    m_pressed[Delete] = sf::Keyboard::isKeyPressed(sf::Keyboard::D);


    auto idiff = imouse - m_mouse;
    m_mouse = imouse;
    if (sf::Mouse::isButtonPressed(sf::Mouse::Middle)) {
        auto diff = WVec(idiff.x, idiff.y);
        auto view = window.getView();
        view.move(-diff);
        engine.set_view(view);
    }

}

void EngineEditorState::update_world() {
    game_world.update_triangles();
}

bool EngineEditorState::load_scene(const std::string &name) {
    using namespace std;
    fstream scene_file(name, ios::in | ios::binary);
    scene::Island pb_island;

    if (!scene_file) {
        cout << "File not found." << endl;
        return false;
    }

    pb_island.ParseFromIstream(&scene_file);
    m_island.clear();
    for (int i = 0 ; i < pb_island.points_size() ; ++i) {
        auto pb_point = pb_island.points(i);
        m_island.m_points.push_back(WVec(pb_point.x(), pb_point.y()));
    }
    for (int i = 0 ; i < pb_island.ctrl_points_size() ; ++i) {
        auto pb_point = pb_island.ctrl_points(i);
        m_island.m_ctrl_points.push_back(WVec(pb_point.x(), pb_point.y()));
    }

    update_world();
    return true;
}

void EngineEditorState::save_scene(const std::string &name) {
    using namespace std;

    scene::Island pb_island;
    for (const auto &point : m_island.m_points) {
        auto pb_point = pb_island.add_points();
        pb_point->set_x(point.x);
        pb_point->set_y(point.y);
    }
    for (const auto &point : m_island.m_ctrl_points) {
        auto pb_point = pb_island.add_ctrl_points();
        pb_point->set_x(point.x);
        pb_point->set_y(point.y);
    }

    fstream  scene_file(name, ios::out | ios::trunc | ios::binary);
    if (!pb_island.SerializeToOstream(&scene_file)) {
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

