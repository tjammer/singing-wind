//
// Created by tobi on 3/28/17.
//

#ifndef SINGING_WIND_EDITOR_H
#define SINGING_WIND_EDITOR_H

#include "Engine.h"
#include "GameWorld.h"
#include <map>

const float point_size = 5;
const float ctrl_point_size = 4;
const float line_size = 2.5f;
const float line_draw_distance = 10.f;

class BaseEditorSubState;

enum EditorButtons {
    Confirm,
    Cancel,
    Move,
    Insert,
    Delete,
    Menu
};

class EngineEditorState : public EngineState {
public:
    void cleanup() override {}

    void update(Engine &engine) override;
    void draw(sf::RenderWindow &window) override ;

    EngineEditorState(const std::string &scene_name, GameWorld& game_world);
    ~EngineEditorState() = default;

private:
    GameWorld &game_world;
    Island &m_island;
    std::unique_ptr<BaseEditorSubState> m_state;
    // last frame
    std::map<EditorButtons, bool> m_pressed;
    sf::Vector2i m_mouse;

    // menu
    bool m_menu = false;
    bool menu();

    void update_world();
    bool load_scene(const std::string &name);
    void save_scene(const std::string &name);
};

class BaseEditorSubState {
public:
    virtual void update(const WVec& mpos) {m_mpos = mpos;};
    virtual void draw(sf::RenderWindow &window) {};
    virtual std::unique_ptr<BaseEditorSubState> confirm(Island &island) = 0;
    virtual std::unique_ptr<BaseEditorSubState> cancel() = 0;
    virtual std::unique_ptr<BaseEditorSubState> move(Island &island) = 0;
    virtual std::unique_ptr<BaseEditorSubState> insert_item(Island &island) = 0;
    virtual std::unique_ptr<BaseEditorSubState> delete_item(Island &island) = 0;
    virtual std::unique_ptr<BaseEditorSubState> menu(Island &island) = 0;

    WVec m_mpos;
};

#endif //SINGING_WIND_EDITOR_H
