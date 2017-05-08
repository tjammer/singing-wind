//
// Created by tobi on 4/26/17.
//

#ifndef SINGING_WIND_ENTITYEDITOR_H
#define SINGING_WIND_ENTITYEDITOR_H

#include "Editor.h"

class EntityIdle : public BaseEditorSubState {
public:
    EditorSubState update(const WVec &mpos) override;
    EditorSubState confirm(GameWorld &world) override;
    EditorSubState cancel() override;
    EditorSubState move(GameWorld &world) override;
    EditorSubState insert_item(GameWorld &world) override;
    EditorSubState delete_item(GameWorld &world) override;
    EditorSubState menu(GameWorld &world) override;
    void draw(GameWorld &world, sf::RenderWindow &window) override;

    EntityIdle(GameWorld &world, unsigned int entity) : m_world(world), m_entity(entity) {}

private:
    GameWorld &m_world;
    unsigned int m_entity;
};


#endif //SINGING_WIND_ENTITYEDITOR_H
