#ifndef LIFETIMECOMPONENT_H
#define LIFETIMECOMPONENT_H

#include "Components.h"
#include <functional>

class GameWorld;

struct LifeTimeComponent
{
  float timer;
  std::function<void(GameWorld&, const unsigned int&)> delete_func = nullptr;
  static const Components type = CLifeTime;
};

#endif /* LIFETIMECOMPONENT_H */
