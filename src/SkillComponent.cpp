#include "SkillComponent.h"
#include "GameWorld.h"
#include <algorithm>

using namespace std;
using skill_func = std::function<void(GameWorld &world, unsigned int entity)>;

std::function<void(GameWorld &world, unsigned int entity)> get_skill_func(const SkillState &, const SkillID &) {
    return nullptr;
}

void cast_skill(GameWorld &world, unsigned int entity, SkillID id) {
    auto &sc = world.skill_c(entity);
    // check if entity is even ready
    if (sc.active != SkillID::None) {
        return;
    }

    // check if skill is in equiped list
    auto it = std::find_if(sc.skills.begin(), sc.skills.end(), [id] (const Skill &s) {return s.id == id;});
    if (it == sc.skills.end()) {
        return;
    }

    // can now activate skill
    sc.active = id;
    (*it).skillstate = SkillState::BuildUp;
    (*it).timer = (*it).c_time_buildup;
}
