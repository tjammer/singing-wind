#ifndef SKILLCOMPONENT_H
#define SKILLCOMPONENT_H

#include <functional>
#include <vector>
#include <map>
#include <unordered_map>

class GameWorld;

enum class SkillState : int {
    Ready,
    Cooldown,
    BuildUp,
    Channel,
    Recover,
    state_count
};

enum class SkillID {
    None,
    Melee,
    state_count
};

struct Skill {
    float c_time_buildup = 1;
    float c_time_channel = 1;
    float c_time_recover = 1;
    float c_time_cooldown = 1;
    SkillState skillstate = SkillState::Ready;
    SkillID id = SkillID::None;
    float timer = 0;
    float cost = 0;
};

struct SkillComponent {
    std::unordered_map<SkillID, Skill> skills;
    SkillID active = SkillID::None;
};

std::function<void(GameWorld &world, unsigned int entity)> get_skill_func(const SkillState &state, const SkillID &id);

void cast_skill(GameWorld &world, unsigned int entity, SkillID id);

const std::map<SkillState, const char*> skillstate_string = {
    {SkillState::Ready, "Ready"},
    {SkillState::Cooldown, "Cooldown"},
    {SkillState::BuildUp, "Buildup"},
    {SkillState::Channel, "Channel"},
    {SkillState::Recover, "Recover"}
};


#endif /* SKILLCOMPONENT_H */
