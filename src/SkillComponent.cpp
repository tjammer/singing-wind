#include "SkillComponent.h"
#include "GameWorld.h"
#include "Melee.h"
#include "Lounge.h"
#include "Components.h"
#include <imgui.h>
#include <algorithm>

using skill_func = std::function<void(GameWorld &world, unsigned int entity)>;
using skill_funcs_t = std::array<skill_func, static_cast<size_t>(SkillState::state_count)>;

namespace skill {
    bool can_cast(GameWorld &world, unsigned int entity, SkillID id) {
        auto &sc = world.skill_c(entity);
        // check if entity is even ready
        if (sc.active != nullptr) {
            return false;
        }

        // check if skill is in equiped list
        auto skill_iterator = std::find_if(sc.skills.begin(), sc.skills.end(),
                [id] (const std::shared_ptr<SkillBase> &ts) {return ts->id == id;});
        if (skill_iterator == sc.skills.end()) {
            return false;
        }

        // check if skill is off cooldown
        if ((*skill_iterator)->skillstate != SkillState::Ready) {
            return false;
        }
        return true;
    }


    bool cast(GameWorld &world, unsigned int entity, SkillID id) {
        if (!can_cast(world, entity, id)) {
            return false;
        }
        auto &sc = world.skill_c(entity);
        auto skill_iterator = std::find_if(sc.skills.begin(), sc.skills.end(),
                [id] (const std::shared_ptr<SkillBase> &ts) {return ts->id == id;});

        // can now activate skill
        auto &skill = *skill_iterator;
        sc.active = skill;
        skill->skillstate = SkillState::BuildUp;
        skill->timer = skill->c_time_buildup;
        skill->buildup_start(world, entity);

        return true;
    }

    const std::map<SkillID, const char *> skillid_string = {
        {SkillID::Melee, "Melee"},
        {SkillID::Lounge, "Lounge"}
    };

    using skillset = std::bitset<static_cast<size_t>(SkillID::state_count)>;

    void entity_edit(GameWorld &world, unsigned int entity) {
        using namespace ImGui;
        if (world.entities()[entity].test(CSkill) and CollapsingHeader("skill")) {
            auto &sc= world.skill_c(entity);

            skillset ss = 0;
            unsigned long skill_flag = 0;
            for (const auto &skill : sc.skills) {
                skill_flag |= (1 << static_cast<int>(skill->id));
            }
            for (auto &pair : skillid_string) {
                CheckboxFlags(pair.second, &skill_flag, 1 << static_cast<int>(pair.first));
            }
            ss = skillset(skill_flag);
            // TODO: save also skill data
            // save skill bitset to array
            sc.skills.clear();
            for (size_t i = 0 ; i < static_cast<size_t>(SkillID::state_count) ; ++i) {
                if (ss.test(i)) {
                    SkillID id = static_cast<SkillID>(i);
                    auto new_skill = skill::get_new_skill(id);
                    if (new_skill) {
                        sc.skills.push_back(skill::get_new_skill(id));
                    }
                }
            }
        }
    }

    std::shared_ptr<SkillBase> get_new_skill(SkillID id) {
        switch (id) {
            case SkillID::Melee : return std::shared_ptr<SkillBase>(new melee_skill::Skill());
            case SkillID::Lounge : return std::shared_ptr<SkillBase>(new lounge_skill::Skill());
            case SkillID::state_count : return nullptr;
        }
        return nullptr;
    }
}
