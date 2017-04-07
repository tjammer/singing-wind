//
// Created by tobi on 3/31/17.
//

#ifndef SINGING_WIND_EDITORSTATES_H
#define SINGING_WIND_EDITORSTATES_H

#include "Editor.h"
#include "BCurve.h"

using EditorSubState = std::unique_ptr<BaseEditorSubState>;

class IslandIdle : public BaseEditorSubState {
public:
    EditorSubState confirm(Island &island) override;
    EditorSubState cancel() override {return EditorSubState(new IslandIdle);}
    EditorSubState move(Island &island) override {return EditorSubState(new IslandIdle);}
    EditorSubState insert_item(Island &island) override {return EditorSubState(new IslandIdle);}
    EditorSubState delete_item(Island &island) override {return EditorSubState(new IslandIdle);}
    EditorSubState menu(Island &island) override;
};

class CurveIdle : public BaseEditorSubState {
public:
    EditorSubState confirm(Island &island) override;
    void draw(sf::RenderWindow &window) override;
    EditorSubState cancel() override {return EditorSubState(new IslandIdle);}
    EditorSubState move(Island &island) override {return EditorSubState(new CurveIdle(m_curve));}
    EditorSubState insert_item(Island &island) override;
    EditorSubState delete_item(Island &island) override {return EditorSubState(new IslandIdle);}
    EditorSubState menu(Island &island) override;

    CurveIdle(const BCurve& curve);
    ~CurveIdle() = default;

private:
    BCurve m_curve;
};

class PointEdit : public BaseEditorSubState {
public:
    void update(const WVec& mpos) override;
    void draw(sf::RenderWindow &window) override;
    EditorSubState confirm(Island &island) override {return EditorSubState(new IslandIdle);}
    EditorSubState cancel() override;
    EditorSubState move(Island &island) override {return EditorSubState(new IslandIdle());}
    EditorSubState insert_item(Island &island) override {return EditorSubState(new IslandIdle());}
    EditorSubState delete_item(Island &island) override;
    EditorSubState menu(Island &island) override;

    PointEdit(WVec &point, const WVec &mouse, WVec &c1, WVec &c2);
    PointEdit(WVec &point, const WVec &mouse);
    ~PointEdit() = default;

private:
    WVec &m_point;
    WVec &m_c1;
    WVec &m_c2;
    WVec m_orig_point;
    bool ctrl = false;
};

class CurveInsert : public BaseEditorSubState {
public:
    void draw(sf::RenderWindow &window) override;
    void update(const WVec& mpos) override;
    EditorSubState confirm(Island &island) override;
    EditorSubState cancel() override {return EditorSubState(new IslandIdle);}
    EditorSubState delete_item(Island &island) override {return EditorSubState(new IslandIdle);}
    EditorSubState insert_item(Island &island) override {return EditorSubState(new IslandIdle);}
    EditorSubState move(Island &island) override {return EditorSubState(new CurveInsert(m_curve));}
    EditorSubState menu(Island &island) override;

    CurveInsert(BCurve curve);
    ~CurveInsert() = default;

private:
    BCurve m_curve;
    float m_new_point_t;
};

#endif //SINGING_WIND_EDITORSTATES_H
