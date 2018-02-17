//
// Created by tobi on 4/26/17.
//

#ifndef SINGING_WIND_ENTITYEDITOR_H
#define SINGING_WIND_ENTITYEDITOR_H

#include "Editor.h"
#include <map>

class EntityIdle : public BaseEditorSubState
{
public:
  EditorSubState update(const WVec& mpos) override;
  void draw(GameWorld& world, float zoom) override;
  EditorSubState move(GameWorld&) override;
  EditorSubState cancel() override;

  EntityIdle(GameWorld& world, unsigned int entity)
    : m_world(world)
    , m_entity(entity)
  {
  }

private:
  GameWorld& m_world;
  unsigned int m_entity;
};

class EntityMove : public BaseEditorSubState
{
public:
  EditorSubState confirm(GameWorld& world) override;
  EditorSubState update(const WVec& mpos) override;
  EditorSubState cancel() override;
  void draw(GameWorld& world, float zoom) override;

  EntityMove(GameWorld& world, unsigned int entity, const WVec& mouse);

private:
  GameWorld& m_world;
  unsigned int m_entity;
  WVec m_diff;
};

template<typename T>
std::vector<const char*>
get_enum_string_array(std::map<T, const char*> data)
{
  std::vector<const char*> strings;

  for (auto& pair : data) {
    strings.push_back(pair.second);
  }
  return strings;
}
#endif // SINGING_WIND_ENTITYEDITOR_H
