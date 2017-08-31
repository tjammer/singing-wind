#ifndef PROTAGONISTMELEE_H
#define PROTAGONISTMELEE_H

#include "SkillComponent.h"

namespace melee_skill {
    void move_buildup(GameWorld&, unsigned int);
    void move_channel(GameWorld &, unsigned int);

    class Skill : public SkillBase {
        public:
            void channel_start(GameWorld &, unsigned int) override;
            void channel_end(GameWorld &, unsigned int) override;
            Skill();
    };
};

#endif /* PROTAGONISTMELEE_H */
