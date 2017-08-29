#include "SkillComponent.h"
#include "GameWorld.h"
#include "Melee.h"
#include "Lounge.h"
#include "Components.h"
#include <imgui.h>
#include <algorithm>
#include <iostream>

using skill_func = std::function<void(GameWorld &world, unsigned int entity)>;
using skill_funcs_t = std::array<skill_func, static_cast<size_t>(SkillState::state_count)>;

namespace skill {
    std::function<void(GameWorld &world, unsigned int entity)> get_func(const SkillState &state, const SkillID &id) {
        switch (id) {
            case SkillID::Melee : {
                return melee_skill::funcs[static_cast<size_t>(state)];
            }
            case SkillID::Lounge : {
                return lounge_skill::funcs[static_cast<size_t>(state)];
            }
            default : return nullptr;
        }
    }

    void cast(GameWorld &world, unsigned int entity, SkillID id) {
        auto &sc = world.skill_c(entity);
        // check if entity is even ready
        if (sc.active != SkillID::None) {
            std::cout << "note ready" << (int)sc.active << std::endl;
            return;
        }

        // check if skill is in equiped list
        if (sc.skills.count(id) == 0) {
            std::cout << "doesnt have skill" << std::endl;
            return;
        }

        // can now activate skill
        sc.active = id;
        sc.skills.at(id).skillstate = SkillState::BuildUp;
        sc.skills.at(id).timer = sc.skills.at(id).c_time_buildup;
    }

    const std::map<SkillID, const char *> skillid_string = {
        {SkillID::None, "None"},
        {SkillID::Melee, "Melee"},
        {SkillID::Lounge, "Lounge"}
    };

    const std::map<SkillID, Skill> skillmap = {
        {SkillID::Melee, melee_skill::skill},
        {SkillID::Lounge, lounge_skill::skill}
    };

    Skill from_id(const SkillID &id) {
        return skillmap.at(id);
    }

    using skillset = std::bitset<static_cast<size_t>(SkillID::state_count)>;

    void entity_edit(GameWorld &world, unsigned int entity) {
        using namespace ImGui;
        if (world.entities()[entity].test(CSkill) and CollapsingHeader("skill")) {
            auto &sc= world.skill_c(entity);

            skillset ss = 0;
            unsigned long skill_flag = 0;
            for (const auto &skill : sc.skills) {
                assert(skill.first == skill.second.id);
                skill_flag |= (1 << static_cast<int>(skill.first));
            }
            for (auto &pair : skillid_string) {
                CheckboxFlags(pair.second, &skill_flag, 1 << static_cast<int>(pair.first));
            }
            ss = skillset(skill_flag);
            // TODO: save also skill data
            // save skill bitset to array
            sc.skills.clear();
            for (size_t i = 1 ; i < static_cast<size_t>(SkillID::state_count) ; ++i) {
                if (ss.test(i)) {
                    SkillID id = static_cast<SkillID>(i);
                    sc.skills[id] = skillmap.at(id); 
                }
            }
        }
    }
}
