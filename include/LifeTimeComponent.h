#ifndef LIFETIMECOMPONENT_H
#define LIFETIMECOMPONENT_H

#include <functional>

class GameWorld;

struct LifeTimeComponent
{
  float timer;
  std::function<void(GameWorld&, const unsigned int&)> delete_func = nullptr;
};

#endif /* LIFETIMECOMPONENT_H */
