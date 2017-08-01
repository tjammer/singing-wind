#ifndef PROTAGONISTMELEE_H
#define PROTAGONISTMELEE_H

#include "SkillComponent.h"
#include <functional>

namespace melee_skill {
    void on_build_up(GameWorld &, unsigned int);
    void on_channel(GameWorld &, unsigned int);
    void on_recover(GameWorld &, unsigned int);

    const Skill skill = {
        .1, .1, 1, 2, SkillState::Ready, SkillID::Melee, 0, 0
    };

    const std::array<std::function<void(GameWorld&, unsigned int)>, static_cast<size_t>(SkillState::state_count)> funcs = {{
        nullptr, // Ready
        nullptr, // Cooldown
        nullptr, // BuildUp
        melee_skill::on_channel, // Channel
        nullptr // Recover
    }};
};

#endif /* PROTAGONISTMELEE_H */
