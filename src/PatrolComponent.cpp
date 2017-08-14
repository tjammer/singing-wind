#include "PatrolComponent.h"
#include "GameWorld.h"
#include "PosComponent.h"
#include "ColShape.h"
#include "CollisionComponent.h"
#include "TagComponent.h"
#include "WindDefs.h"
#include "LifeTimeComponent.h"
#include "Components.h"
#include "AIComponent.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_transform_2d.hpp>

namespace alert_bubble {
    void spawn(GameWorld &world, unsigned int entity) {
        
        // spawn alert bubble
        auto alert = world.create_entity();
        bset comps;
        for (auto i : {CPosition, CColShape, CDynCol, CDebugDraw, CTag}) {
            comps.set(i);
        }
        world.entities()[alert] = comps;
        world.name_c(alert) = "alert bubble";

        // pos
        auto &pc = world.pos_c(alert);
        pc.parent = entity;
        pc.position = {0, 0};
pc.global_transform =  world.pos_c(pc.parent).global_transform * glm::rotate(glm::translate(WTransform(), pc.position), pc.rotation);


        // col shape
        auto &csc = world.cshape_c(alert);
        csc.shape = std::shared_ptr<ColShape>(new ColCircle(400));

        // tags
        auto &tc = world.tag_c(alert);
        tc.set(static_cast<int>(Tags::AlertBubble));

        // dyn col
        auto &dc = world.dyn_col_c(alert);
        dc.col_response = DynColResponse::AlertBubble;
    }

    void on_dynamic_collision(GameWorld &world, unsigned int entity) {
        // test tag comp of collider
        auto &dc = world.dyn_col_c(entity);
        auto &tc = world.tag_c(dc.collided);
        if (!tc.test(static_cast<int>(Tags::Protagonist))) {
            return;
        }
        // test if was updated -> still in idle
        auto &pc = world.pos_c(entity);
        auto &parent_ac = world.ai_c(pc.parent);
        if (world.ai_c(pc.parent).msg_data.size() > 0) {
            // last entry is alert bubble entity
            parent_ac.msg_data.pop_back();
        }
        parent_ac.msg_data.push_back(static_cast<unsigned int>(dc.collided));
        parent_ac.msg_data.push_back(static_cast<unsigned int>(entity));
    }
}
