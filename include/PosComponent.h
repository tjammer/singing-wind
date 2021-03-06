#ifndef POSCOMPONENT_H
#define POSCOMPONENT_H

#include "WindDefs.h"
#include "Components.h"

struct PosComponent
{
  WVec position = { 0, 0 };
  WVec global_position = { 0, 0 };
  float rotation = 0.f;
  WTransform global_transform = WTransform();
  unsigned int parent = 0;
  int direction = 1;
  static const Components type = CPosition;

  // returns wrapped global position
  WVec wrapped_position(const WVec& other);
};

class GameWorld;
void
build_global_transform(GameWorld&, unsigned int);
void
rotate_to(const WVec& to, float max_angle, PosComponent& pc);
void
rotate_angle(float angle, float max_angle, PosComponent& pc);
#endif /* POSCOMPONENT_H */
