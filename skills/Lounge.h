#ifndef LOUNGE_H
#define LOUNGE_H

#include "SkillComponent.h"
#include <functional>

namespace lounge_skill {
    void on_buildup(GameWorld &, unsigned int);
    void on_channel(GameWorld &, unsigned int);
    void on_recover(GameWorld &, unsigned int);
    void on_cooldown(GameWorld &, unsigned int);

    void move_buildup(GameWorld &, unsigned int);
    void move_channel(GameWorld &, unsigned int);

    const Skill skill = {
        .5, .3, 1, 5, SkillState::Ready, SkillID::Lounge, 0, 0
    };

    const std::array<std::function<void(GameWorld &, unsigned int)>, static_cast<size_t>(SkillState::state_count)> funcs = {{
        nullptr, // Ready
        lounge_skill::on_cooldown, // Cooldown
        lounge_skill::on_buildup, // BuildUp
        lounge_skill::on_channel, // Channel
        nullptr // Recover
    }};
}

#endif /* LOUNGE_H */
