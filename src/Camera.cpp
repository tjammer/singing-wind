//
// Created by tobi on 4/19/17.
//

#include "Camera.h"

void Camera::update(GameWorld &game_world, Engine &engine) {
    const sf::RenderWindow&  window = engine.get_window();
    auto view = window.getView();
    view.setCenter(game_world.m_pos_c[entity_to_follow].position);
    engine.set_view(view);
}
