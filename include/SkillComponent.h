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

enum class SkillID : int {
    None,
    Melee,
    Lounge,
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

namespace skill {
    std::function<void(GameWorld &world, unsigned int entity)> get_func(const SkillState &state, const SkillID &id);

    void cast(GameWorld &world, unsigned int entity, SkillID id);

    void entity_edit(GameWorld &, unsigned int);

    void add(GameWorld &, unsigned int);

    Skill from_id(const SkillID &);
}

#endif /* SKILLCOMPONENT_H */
