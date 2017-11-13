#ifndef SKILLCOMPONENT_H
#define SKILLCOMPONENT_H

#include <functional>
#include <vector>
#include <memory>

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
    Melee,
    Lounge,
    state_count
};

class SkillBase {
    public:
        const float c_time_buildup = 1;
        const float c_time_channel = 1;
        const float c_time_recover = 1;
        const float c_time_cooldown = 1;
        SkillState skillstate = SkillState::Ready;
        SkillID id;
        float cost = 0;
        float timer = 0;
        int frame_time = 0; // counts frames for timers

        virtual void buildup_start(GameWorld &, unsigned int) {}
        virtual void buildup_tick(GameWorld &, unsigned int) {}
        virtual void buildup_end(GameWorld &, unsigned int) {}
        virtual void channel_start(GameWorld &, unsigned int) {}
        virtual void channel_tick(GameWorld &, unsigned int) {}
        virtual void channel_end(GameWorld &, unsigned int) {}
        virtual void recover_start(GameWorld &, unsigned int) {}
        virtual void recover_tick(GameWorld &, unsigned int) {}
        virtual void recover_end(GameWorld &, unsigned int) {}

        virtual void move_buildup(GameWorld &, unsigned int) {}
        virtual void move_channel(GameWorld &, unsigned int) {}
        virtual void move_recover(GameWorld &, unsigned int) {}

        SkillID get_id() const {return id;}
    protected:
        SkillBase(float buildup, float channel, float recover, float cooldown, SkillID id) :
            c_time_buildup(buildup), c_time_channel(channel), c_time_recover(recover), c_time_cooldown(cooldown), id(id) {}
};

struct SkillComponent {
    std::vector<std::shared_ptr<SkillBase>> skills;
    std::shared_ptr<SkillBase> active = nullptr;
    std::vector<int> skill_data;
};

namespace skill {
    bool can_cast(GameWorld &, unsigned int, SkillID);
    bool cast(GameWorld &world, unsigned int entity, SkillID id);

    void entity_edit(GameWorld &, unsigned int);

    void add(GameWorld &, unsigned int);

    std::shared_ptr<SkillBase> get_new_skill(SkillID id);

    void reset(GameWorld&, unsigned int);
}

#endif /* SKILLCOMPONENT_H */
