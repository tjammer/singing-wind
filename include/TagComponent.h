#ifndef TAGCOMPONENT_H
#define TAGCOMPONENT_H

#include "WindDefs.h"
#include "Components.h"

class GameWorld;

struct TagComponent
{
  bset tags;
  static const Components type = CTag;
};

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
