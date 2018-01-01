#ifndef TAGCOMPONENT_H
#define TAGCOMPONENT_H

#include "WindDefs.h"

class GameWorld;

using TagComponent = bset;

enum class Tags : int
{
  Actor,
  Protagonist,
  Enemy,
  Projectile,
  Hurtbox,
  AlertBubble
};

// for editor
void
entity_edit_tags(GameWorld&, unsigned int entity);

#endif /* TAGCOMPONENT_H */
