//
// Created by tobi on 3/27/17.
//

#ifndef SINGING_WIND_STATEBASE_H
#define SINGING_WIND_STATEBASE_H

#include "SFML/Graphics.hpp"
#include <vector>
#include <memory>

class EngineState;

// engine base class, different from gameplay states
class Engine {
public:
    void init();

    void change_state(EngineState& state);
    void push_state(EngineState& state);
    void pop_state();

    void update();
    void draw();

    Engine(sf::RenderWindow& window);
    ~Engine() {}

    const sf::RenderWindow& get_window() const {return window;}

    void set_focus(bool focus) {m_has_focus = focus;}
    bool get_focus() const {return m_has_focus;}

    void set_view(const sf::View &view);

    void editor_to_game();
private:
    bool m_has_focus = true;
    std::vector<std::unique_ptr<EngineState>> m_states;
    sf::RenderWindow& window;
    size_t m_game_index = std::numeric_limits<long>::max();
    size_t m_editor_index = std::numeric_limits<long>::max();

    // switch
    bool m_switch_pressed = false;
};



class EngineState {
public:
    virtual void cleanup() = 0;

    virtual void pause() { m_paused = true;}
    virtual void unpause() {m_paused = false;}
    virtual void switch_pause() {m_paused = !m_paused;}

    virtual void update(Engine& engine) = 0;
    virtual void draw(sf::RenderWindow& window) = 0;

    virtual ~EngineState() = default;

    bool m_paused = false;
};


#endif //SINGING_WIND_STATEBASE_H
