//
// Created by tobi on 4/19/17.
//

#include "Camera.h"

void Camera::update(GameWorld &game_world, Engine &engine) {
    const sf::RenderWindow&  window = engine.get_window();
    auto view = window.getView();
    view.setCenter({(game_world.m_pos_c[m_entity_to_follow].position).x, (game_world.m_pos_c[m_entity_to_follow].position).y});
    engine.set_view(view);
}
