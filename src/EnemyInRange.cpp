#include "EnemyInRange.h"
#include "ColGrid.h"
#include "GameWorld.h"
#include "PosComponent.h"
#include "TagComponent.h"
#include "Pathfinding.h"

behaviour_tree::Status EnemyInRange::update() {
    using namespace behaviour_tree;
    const auto &pos = m_world.pos_c(m_entity).global_position;
    auto colliders = m_world.dynamic_grid().find_colliders_in_radius(pos, m_radius);

    for (auto &col : colliders) {
        if (m_world.tag_c(col.entity).test(static_cast<int>(Tags::Protagonist))) {
            m_path_c.following = col.entity;
            return Status::Success;
        }
    }
    
    return Status::Failure;
}
