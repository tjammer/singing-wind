#include "SkillComponent.h"
#include "GameWorld.h"
#include "Melee.h"
#include <algorithm>

using skill_func = std::function<void(GameWorld &world, unsigned int entity)>;
using skill_funcs_t = std::array<skill_func, static_cast<size_t>(SkillState::state_count)>;

std::function<void(GameWorld &world, unsigned int entity)> get_skill_func(const SkillState &state, const SkillID &id) {
    switch (id) {
        case SkillID::Melee : {
            return melee_skill::funcs[static_cast<size_t>(state)];
        }
        default : return nullptr;
    }
}

void cast_skill(GameWorld &world, unsigned int entity, SkillID id) {
    auto &sc = world.skill_c(entity);
    if (sc.skills.size() == 0) {
        sc.skills[melee_skill::skill.id] = melee_skill::skill;
    }
    // check if entity is even ready
    if (sc.active != SkillID::None) {
        return;
    }

    // check if skill is in equiped list
    if (sc.skills.count(id) == 0) {
        return;
    }

    // can now activate skill
    sc.active = id;
    sc.skills.at(id).skillstate = SkillState::BuildUp;
    sc.skills.at(id).timer = sc.skills.at(id).c_time_buildup;
}
