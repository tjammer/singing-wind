#ifndef STATUSEFFECTHITSTUN_H
#define STATUSEFFECTHITSTUN_H

#include "MoveSystems.h"
#include "StatusEffectComponent.h"

class Hitstun : public StatusEffect
{
public:
  void enter(GameWorld&, unsigned int) override;
  void tick(GameWorld&, unsigned int) override;
  void leave(GameWorld&, unsigned int) override;
  Hitstun(float timer)
    : StatusEffect(StatusEffectName::HitStun, timer)
  {
  }
};

#endif /* STATUSEFFECTHITSTUN_H */
