#ifndef LOUNGE_H
#define LOUNGE_H

#include "SkillComponent.h"

namespace lounge_skill {
    void move_buildup(GameWorld &, unsigned int);
    void move_channel(GameWorld &, unsigned int);

    class Skill : public SkillBase {
        public:
            void buildup_start(GameWorld &, unsigned int) override;
            void buildup_end(GameWorld &, unsigned int) override;
            void channel_start(GameWorld &, unsigned int) override;
            void channel_end(GameWorld &, unsigned int) override;
            void recover_start(GameWorld &, unsigned int) override;
            void recover_end(GameWorld &, unsigned int) override;
            Skill();
    };
}

#endif /* LOUNGE_H */
