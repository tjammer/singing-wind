//
// Created by tobi on 3/28/17.
//

#include "Editor.h"
#include "EditorStates.h"
#include "triangulate.h"

void EngineEditorState::draw(sf::RenderWindow &window) {
    if (m_paused) {
        return;
    }
    using namespace sf;

    auto curves = m_island.get_curves(line_draw_distance);
    VertexArray va(Quads);
    for (auto& v : curves) {
        va.append(v);
    }

    auto verts = m_island.get_points(point_size);
    for (auto& v : verts) {
        va.append(v);
    }

    if (m_triangulate) {
        // triangles
        VertexArray tris(Lines);
        for (uint i = 0 ; i < m_triangles.size() / 3 ; ++i) {
            tris.append(Vertex(m_triangles[i*3], Color::White));
            tris.append(Vertex(m_triangles[i*3+1], Color::White));
            tris.append(Vertex(m_triangles[i*3+1], Color::White));
            tris.append(Vertex(m_triangles[i*3+2], Color::White));
            tris.append(Vertex(m_triangles[i*3+2], Color::White));
            tris.append(Vertex(m_triangles[i*3], Color::White));
        }
        window.draw(tris);
    }

    window.draw(va);
    m_state->draw(window);
}

EngineEditorState::EngineEditorState(){
    m_island = Island(WVec(0, 0), 150);
    m_state = std::unique_ptr<BaseEditorSubState>(new IslandIdle);
    triangulate_island(m_island, m_triangles);
}

void EngineEditorState::update(Engine &engine) {
    if (!engine.get_focus()) {
        return;
    }
    const sf::RenderWindow& window = engine.get_window();
    auto imouse = sf::Mouse::getPosition(window);
    auto idiff = imouse - m_mouse;
    m_mouse = imouse;
    auto mouse = window.mapPixelToCoords(imouse);
    auto diff = WVec(idiff.x, idiff.y);

    m_state->update(mouse);

    if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right) and not m_confirm_pressed) {
        m_state = m_state->confirm(m_island);
        triangulate_island(m_island, m_triangles);
    }
    m_confirm_pressed = sf::Mouse::isButtonPressed(sf::Mouse::Button::Right);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape) and not m_cancel_pressed) {
        m_state = m_state->cancel();
    }
    m_cancel_pressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Escape);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::G) and not m_move_pressed) {
        m_state = m_state->move(m_island);
    }
    m_move_pressed = sf::Keyboard::isKeyPressed(sf::Keyboard::G);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::I) and not m_insert_pressed) {
        m_state = m_state->insert_curve(m_island);
        triangulate_island(m_island, m_triangles);
    }
    m_insert_pressed = sf::Keyboard::isKeyPressed(sf::Keyboard::I);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) and not m_delete_pressed) {
        m_state = m_state->delete_curve(m_island);
        triangulate_island(m_island, m_triangles);
    }
    m_delete_pressed = sf::Keyboard::isKeyPressed(sf::Keyboard::D);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::F12) and not m_triangulate_pressed) {
        m_triangulate = !m_triangulate;
    }
    m_triangulate_pressed = sf::Keyboard::isKeyPressed(sf::Keyboard::F12);


    if (sf::Mouse::isButtonPressed(sf::Mouse::Middle)) {
        auto view = window.getView();
        view.move(-diff);
        engine.set_view(view);
    }

}

