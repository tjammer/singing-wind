//
// Created by tobi on 3/27/17.
//

#include "Engine.h"
#include "Editor.h"
#include "Game.h"

Engine::Engine(sf::RenderWindow &window) : window(window) {
    m_states.emplace_back(new Game);
    m_game_index = m_states.size() - 1;
    m_states.emplace_back(new EngineEditorState("scenes/debug.wscn", get_world()));
    m_editor_index = m_states.size() - 1;
    // transition to editor
    m_states[m_game_index]->pause();
    auto view = window.getView();
    view.setCenter(0, 0);
    window.setView(view);
}

void Engine::update() {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::F1) and not m_switch_pressed) {
        m_states[m_game_index]->switch_pause();
        m_states[m_editor_index]->switch_pause();
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

GameWorld &Engine::get_world() {
    return (*dynamic_cast<Game*>(m_states[m_game_index].get())).get_world();
}
