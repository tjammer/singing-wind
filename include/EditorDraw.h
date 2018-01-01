#ifndef EDITORDRAW_H
#define EDITORDRAW_H

#include <bitset>

class GameWorld;

enum class EditorDrawables : int
{
  EntityPos,
  Pathfinding,
  AttentionZone,
};

using EditorDrawSet = std::bitset<static_cast<int>(3)>;

namespace drawables {
void
draw(const EditorDrawSet& drawset, GameWorld& world);
void
menu(EditorDrawSet& drawset);
}

#endif /* EDITORDRAW_H */
