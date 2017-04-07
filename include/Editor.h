//
// Created by tobi on 3/28/17.
//

#ifndef SINGING_WIND_EDITOR_H
#define SINGING_WIND_EDITOR_H

#include "Engine.h"
#include "Island.h"

const float point_size = 5;
const float ctrl_point_size = 4;
const float line_size = 2.5f;
const float line_draw_distance = 10.f;

class BaseEditorSubState;

class EngineEditorState : public EngineState {
public:
    void cleanup() override {}

    void update(Engine &engine) override;
    void draw(sf::RenderWindow &window) override ;

    EngineEditorState();
    ~EngineEditorState() = default;

    const std::vector<WVec>& get_triangles() const {return m_triangles;}

private:
    Island m_island;
    std::unique_ptr<BaseEditorSubState> m_state;
    // last frame
    bool m_confirm_pressed = false;
    bool m_cancel_pressed = false;
    bool m_move_pressed = false;
    bool m_insert_pressed = false;
    bool m_delete_pressed = false;
    bool m_triangulate_pressed = false;
    sf::Vector2i m_mouse;

    // menu
    bool m_menu_pressed = false;
    bool m_menu = false;

    bool m_triangulate = false;
    std::vector<WVec> m_triangles;

    void update_world(Engine &engine);
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
