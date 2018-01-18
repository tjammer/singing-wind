#ifndef STATUSEFFECTCOMPONENT_H
#define STATUSEFFECTCOMPONENT_H

#include <vector>
#include <memory>

class GameWorld;

enum class StatusEffectName
{
  Knockback,
  HitStun
};

class StatusEffect
{
public:
  virtual void enter(GameWorld&, unsigned int) = 0;
  virtual void tick(GameWorld&, unsigned int) = 0;
  virtual void leave(GameWorld&, unsigned int) = 0;
  StatusEffectName name() const { return m_name; }
  float timer;

protected:
  StatusEffect(StatusEffectName name, float timer)
    : timer(timer)
    , m_name(name)
  {
  }

private:
  StatusEffectName m_name;
};

struct StatusEffectComponent
{
  std::vector<std::shared_ptr<StatusEffect>> effects;
};

namespace statuseffects {

void
add_effect(GameWorld&,
           unsigned int entity,
           std::shared_ptr<StatusEffect> effect);
void
delete_effect(StatusEffectComponent& status_c,
              std::shared_ptr<StatusEffect> effect);

void
knockback_move(GameWorld&, unsigned int);
};

#endif /* STATUSEFFECTCOMPONENT_H */
