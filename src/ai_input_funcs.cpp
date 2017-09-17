#include "ai_input_funcs.h"
#include "GameWorld.h"
#include "MoveSystems.h"
#include "InputComponent.h"
#include "Pathfinding.h"
#include "PatrolComponent.h"
#include "WVecMath.h"
#include "SkillComponent.h"
#include "AIComponent.h"
#include "PosComponent.h"

namespace ai_input {

    void simple_flying(GameWorld &world, unsigned int entity) {
        auto &ic = world.input_c(entity);
        auto &pc = world.path_c(entity);
        pc.p_type = PathingType::Fly;
        if (pc.path.size() > pc.index) {
            push_value(ic.mouse, pc.path[pc.index]);
        }
    }

    void hover(GameWorld &world, unsigned int entity) {
        auto &ic = world.input_c(entity);
        auto &pc = world.patrol_c(entity);
        push_value(ic.mouse, pc.patrol_point);
    }

    void attack(GameWorld &world, unsigned int entity) {
        auto &ac = world.ai_c(entity);
        assert(ac.msg_data[0] == (int)(SkillID::Lounge));
        auto &ic = world.input_c(entity);
        push_value(ic.mouse, world.pos_c(world.path_c(entity).following).position);
    }
}
