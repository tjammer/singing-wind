//
// Created by tobi on 3/27/17.
//

#include "Engine.h"
#include "Editor.h"
#include "CollisionTest.h"

Engine::Engine(sf::RenderWindow &window) : window(window) {
    m_states.emplace_back(new CollisionTestState);
    m_states.emplace_back(new EngineEditorState);
    EngineEditorState* ees = dynamic_cast<EngineEditorState*>(m_states[1].get());
    CollisionTestState* cts = dynamic_cast<CollisionTestState*>(m_states[0].get());
    m_states[0]->pause();
    cts->receive_tris(ees->get_triangles());
    auto view = window.getView();
    view.setCenter(0, 0);
    window.setView(view);
}

void Engine::update() {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::F1) and not m_switch_pressed) {
        m_states[0]->switch_pause();
        m_states[1]->switch_pause();
        if (m_states[1]->m_paused) {
            // update tris

            EngineEditorState* ees = dynamic_cast<EngineEditorState*>(m_states[1].get());
            CollisionTestState* cts = dynamic_cast<CollisionTestState*>(m_states[0].get());
            cts->receive_tris(ees->get_triangles());
        }
    }
    m_switch_pressed = sf::Keyboard::isKeyPressed(sf::Keyboard::F1);


    for (auto& state : m_states) {
        if (!state->m_paused) {
            state->update(*this);
        }
    }
    draw();
}

void Engine::draw() {
    window.clear(sf::Color(43, 48, 59));
    for (auto& state: m_states) {
        state->draw(window);
    }
}

void Engine::set_view(const sf::View &view) {
    window.setView(view);
}
