#ifndef SKILLCOMPONENT_H
#define SKILLCOMPONENT_H

#include "Components.h"
#include <functional>
#include <vector>
#include <memory>

class GameWorld;

enum class SkillState : int
{
  Ready,
  Cooldown,
  Active
};

enum class SkillID : int
{
  Melee,
  Lounge,
  Disk,
  state_count
};

struct MoveComponent;
class BaseSkill
{
public:
  float timer;
  SkillID get_id() const { return id; }
  float get_t_active() const { return t_active; }
  float get_t_cooldown() const { return t_cooldown; }
  SkillState state = SkillState::Ready;
  virtual void set_special(
    MoveComponent& move_c) = 0; // sets correct timedmovestate
protected:
  BaseSkill(SkillID id, float active, float cooldown)
    : id(id)
    , t_active(active)
    , t_cooldown(cooldown)
  {
  }

private:
  SkillID id;
  float damage;
  float t_active;
  float t_cooldown;
};

const unsigned int c_skill_slots = 4;

struct SkillComponent
{
  std::vector<std::shared_ptr<BaseSkill>> skills;
  std::shared_ptr<BaseSkill> active = nullptr;
  std::vector<int> skill_data;
  static const Components type = CSkill;
};

namespace skill {
bool
can_cast(GameWorld&, unsigned int, SkillID);
bool
cast(GameWorld& world, unsigned int entity, SkillID id);
void
entity_edit(GameWorld&, unsigned int);
void
add(GameWorld&, unsigned int);
std::shared_ptr<BaseSkill>
get_new_skill(SkillID id);
void
reset(GameWorld&, unsigned int);
}

#endif /* SKILLCOMPONENT_H */
