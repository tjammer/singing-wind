#ifndef SKILLCOMPONENT_H
#define SKILLCOMPONENT_H

#include <string>
#include <vector>

enum class SkillState {
    Ready,
    Cooldown,
    BuildUp,
    Channel,
    Recover
};

struct Skill {
    float c_time_buildup = 1;
    float c_time_channel = 1;
    float c_time_recover = 1;
    float c_cooldown = 1;
    SkillState skillstate = SkillState::Ready;
    std::string name = "skill";
};

struct SkillComponent {
    std::vector<Skill> skills;
    int active = -1;
};


#endif /* SKILLCOMPONENT_H */
