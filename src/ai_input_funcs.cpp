#include "ai_input_funcs.h"
#include "GameWorld.h"
#include "MoveSystems.h"
#include "InputComponent.h"
#include "Pathfinding.h"
#include "WVecMath.h"

namespace ai_input {

    void simple_flying(GameWorld &world, unsigned int entity) {
        auto &ic = world.input_c(entity);
        auto &pc = world.path_c(entity);
        pc.p_type = PathingType::Fly;
        if (pc.path.size() > pc.index) {
            push_value(ic.mouse, pc.path[pc.index]);
        }
    }
}
