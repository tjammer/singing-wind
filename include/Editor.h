//
// Created by tobi on 3/28/17.
//

#ifndef SINGING_WIND_EDITOR_H
#define SINGING_WIND_EDITOR_H

#include "Engine.h"
#include "GameWorld.h"
#include "../target/release/Scene.pb.h"
#include <unordered_map>

const float c_point_size = 5;
const float c_ctrl_point_size = 4;
const float c_line_size = 2.5f;
const float c_line_draw_distance = 10.f;

class BaseEditorSubState;

enum EditorButtons {
    Confirm,
    Cancel,
    Move,
    Insert,
    Delete,
    Menu,
    ResetZoom
};

class EngineEditorState : public EngineState {
public:
    void cleanup() override {}

    void update(Engine &engine) override;
    void draw(sf::RenderWindow &window) override ;

    EngineEditorState(const std::string &scene_name, GameWorld &game_world);
    ~EngineEditorState() = default;

private:
    GameWorld &m_game_world;
    std::unique_ptr<BaseEditorSubState> m_state;
    // last frame
    std::unordered_map<EditorButtons, bool> m_pressed;
    sf::Vector2i m_mouse;
    float m_mouse_wheel = 0;

    // menu
    bool m_menu = false;
    bool main_menu();

    void update_world();
    bool load_scene(const std::string &name);
    void save_scene(const std::string &name) const;

    // zoom
    float m_zoom = 1.f;
    bool m_update_view = false;
};

using EditorSubState = std::unique_ptr<BaseEditorSubState>;

class BaseEditorSubState {
public:
    virtual EditorSubState update(const WVec &mpos) { m_mpos = mpos; return nullptr;}
    virtual void draw(GameWorld &world, sf::RenderWindow &window) = 0;
    virtual EditorSubState confirm(GameWorld &) {return nullptr;}
    virtual EditorSubState cancel() {return nullptr;}
    virtual EditorSubState move(GameWorld &) {return nullptr;}
    virtual EditorSubState insert_item(GameWorld &) {return nullptr;}
    virtual EditorSubState delete_item(GameWorld &) {return nullptr;}
    virtual EditorSubState menu(GameWorld &) {return nullptr;}

    WVec m_mpos;
};


bool load_entity_from_filename(const std::string &name, GameWorld &game_world, unsigned int entity);
scene::Entity * get_pb_entity(const GameWorld &game_world, unsigned int entity);
bool save_entity_standalone(const GameWorld &game_world, unsigned int entity);
void delete_entity_from_scene(GameWorld &game_world, unsigned int entity);
void entity_to_world(const scene::Entity& entity, GameWorld &game_world, unsigned int);

#endif //SINGING_WIND_EDITOR_H
