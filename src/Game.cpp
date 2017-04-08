//
// Created by jammer on 08/04/17.
//

#include "Game.h"

Game::Game() {

}

void Game::update(Engine &engine) {
    if (!engine.get_focus()) {
        return;
    }
    const sf::RenderWindow &window = engine.get_window();
    auto mouse = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    game_world.step_fixed(mouse);
}

void Game::draw(sf::RenderWindow &window) {
    game_world.draw(window);
}
