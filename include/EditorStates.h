//
// Created by tobi on 3/31/17.
//

#ifndef SINGING_WIND_EDITORSTATES_H
#define SINGING_WIND_EDITORSTATES_H

#include "Editor.h"
#include "BCurve.h"

class Island;

class IslandIdle : public BaseEditorSubState {
public:
    EditorSubState confirm(GameWorld &world) override;
    EditorSubState cancel() override;
    EditorSubState move(GameWorld &world) override;
    // extra fct so only do this per mouse in the menu
    EditorSubState delete_island(GameWorld &world);
    EditorSubState menu(GameWorld &world) override;
    void draw(GameWorld &world) override;

    IslandIdle(Island &active) : m_island(active) {}
    ~IslandIdle() = default;

private:
    Island &m_island;
};

class IslandMove : public BaseEditorSubState {
public:
    EditorSubState update(const WVec &mpos) override;
    EditorSubState confirm(GameWorld &) override {return EditorSubState(new IslandIdle(m_island));}
    EditorSubState cancel() override;
    EditorSubState menu(GameWorld &world) override;
    void draw(GameWorld &world) override;

    IslandMove(Island &active, const WVec &mouse);
    ~IslandMove() = default;

private:
    Island &m_island;
    WVec m_diff;

};

class CurveIdle : public BaseEditorSubState {
public:
    EditorSubState confirm(GameWorld &world) override;
    void draw(GameWorld &world) override;
    EditorSubState cancel() override {return EditorSubState(new IslandIdle(m_island));}
    EditorSubState move(GameWorld &) override {return EditorSubState(new CurveIdle(m_curve, m_island));}
    EditorSubState insert_item(GameWorld &world) override;
    EditorSubState menu(GameWorld &world) override;
    void print_formatted_bezier();

    CurveIdle(const BCurve &curve, Island &active);
    ~CurveIdle() = default;

private:
    BCurve m_curve;
    Island &m_island;
};

class PointEdit : public BaseEditorSubState {
public:
    EditorSubState update(const WVec &mpos) override;
    void draw(GameWorld &world) override;
    EditorSubState confirm(GameWorld &) override {return EditorSubState(new IslandIdle(m_island));}
    EditorSubState cancel() override;
    EditorSubState delete_item(GameWorld &world) override;
    EditorSubState menu(GameWorld &world) override;

    PointEdit(WVec &point, const WVec &mouse, WVec &c1, WVec &c2, Island &active);
    PointEdit(WVec &point, const WVec &mouse, Island &active);
    ~PointEdit() = default;

private:
    WVec &m_point;
    WVec &m_c1;
    WVec &m_c2;
    WVec m_orig_point;
    bool m_ctrl = false;
    Island &m_island;
};

class CurveInsert : public BaseEditorSubState {
public:
    void draw(GameWorld &world) override;
    EditorSubState update(const WVec &mpos) override;
    EditorSubState confirm(GameWorld &world) override;
    EditorSubState cancel() override {return EditorSubState(new IslandIdle(m_island));}
    EditorSubState menu(GameWorld &world) override;

    CurveInsert(BCurve curve, Island &active);
    ~CurveInsert() = default;

private:
    BCurve m_curve;
    float m_new_point_t;
    Island &m_island;
};

class EditorIdle : public BaseEditorSubState {
public:
    void draw(GameWorld &world) override;
    EditorSubState confirm(GameWorld &world) override;
    EditorSubState insert_item(GameWorld &world) override;
    EditorSubState menu(GameWorld &world) override;
};

void get_island_vertex_array(const Island&);
#endif //SINGING_WIND_EDITORSTATES_H
