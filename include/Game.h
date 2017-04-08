//
// Created by jammer on 08/04/17.
//

#ifndef SINGING_WIND_GAME_H
#define SINGING_WIND_GAME_H

#include "Engine.h"
#include "GameWorld.h"

// the game state of the engine FSM
class Game : public EngineState {
public:
    virtual void cleanup() override {};

    virtual void update(Engine& engine) override;
    virtual void draw(sf::RenderWindow& window) override;

    Game();
    ~Game() = default;

    bool m_paused = false;

    GameWorld &get_world() {return game_world;};

private:
    GameWorld game_world;
};


#endif //SINGING_WIND_GAME_H
