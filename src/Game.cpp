//
// Created by jammer on 08/04/17.
//

#include "Game.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "WInput.h"

Game::Game() {
    update_camera_follow();
    m_camera.update(m_game_world);
}

void Game::update(Engine &engine) {
    if (!engine.get_focus()) {
        return;
    }
    GLFWwindow &window = engine.get_window();
    double mpos[2];
    glfwGetCursorPos(&window, &mpos[0], &mpos[1]);

    auto mouse = m_camera.unproject_mouse(mpos);

    m_timer.update();
    while (m_timer.pop_fixed()) {
        WInput::set_mouse(mouse);
        m_game_world.step_fixed(c_fixed_timestep);
    }
    m_camera.update(m_game_world);
}

void Game::draw() {
    m_game_world.draw();
}

void Game::unpause() {
    EngineState::unpause();
    m_timer.reset();
    update_camera_follow();

}

void Game::update_camera_follow() {
    for (unsigned int i = 0 ; i < m_game_world.entities().size() ; ++i) {
        if (m_game_world.name_c(i) == "player" and m_game_world.entities()[i].any()) {
            m_camera.m_entity_to_follow = i;
        }
    }
}
