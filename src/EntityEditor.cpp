//
// Created by tobi on 4/26/17.
//

#include "EntityEditor.h"
#include "EditorStates.h"
#include "GameWorld.h"
#include "ColShape.h"
#include "imgui.h"
#include "imgui-bezier.h"
#include "SceneIO.h"
#include "Protagonist.h"
#include "PosComponent.h"
#include "Components.h"
#include "InputComponent.h"
#include "CollisionComponent.h"
#include "MoveSystems.h"
#include "Pathfinding.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_transform_2d.hpp>

template<typename T>
std::vector<const char*> get_enum_string_array(std::map<T, const char*> data) {
    std::vector<const char*> strings;

    for (auto &pair : data) {
        strings.push_back(pair.second);
    }
    return strings;
}

auto moveset_names = get_enum_string_array(moveset_string);
auto movestate_names = get_enum_string_array(movestate_string);
const char* const colshape_names = {"Triangle\0Circle\0Capsule\0\0"};
auto col_responses = get_enum_string_array(staticcolresponse_string);
auto inputfunc_names = get_enum_string_array(inputfunc_string);

void EntityIdle::draw(GameWorld &world) {
    bset debug_draw; debug_draw.set(CPosition); debug_draw.set(CDebugDraw);

    if (!for_gameworld::has_component(world.entities()[m_entity], debug_draw))  {
        return;
    }
    const auto &shape = world.cshape_c(m_entity).shape;
    auto circle = ColCircle(shape->get_radius());
    circle.m_highlight = true;
    circle.add_gfx_lines(world.pos_c(m_entity).global_transform);
}

EditorSubState EntityIdle::update(const WVec &mpos) {
    using namespace std;
    using namespace ImGui;
    BaseEditorSubState::update(mpos);

    EditorSubState transition = nullptr;

    Begin("Entity Editor");
    // base information
    Text("id:");
    SameLine(100);
    Text(to_string(m_entity).c_str());

    string &_name = m_world.name_c(m_entity);
    vector<char> entity_name(_name.begin(), _name.end());
    entity_name.push_back('\0');
    entity_name.resize(128);
    InputText("name", &entity_name[0], entity_name.size());
    _name = string(&entity_name[0]);

    // bitset
    if (CollapsingHeader("components bitset")) {
        auto &comps = m_world.entities()[m_entity];

        auto flags = comps.to_ulong();
        for (auto &pair : components_string) {
            CheckboxFlags(pair.second, &flags, 1 << static_cast<int>(pair.first));
        }
        comps = bset(flags);
        Text(comps.to_string().c_str());
    }

    // position
    if (m_world.entities()[m_entity].test(CPosition) and CollapsingHeader("position")) {
        auto &pc = m_world.pos_c(m_entity);
        float data[2] = {pc.position.x, pc.position.y};
        if (DragFloat2("position", data)) {
            pc.position.x = data[0];
            pc.position.y = data[1];pc.global_transform = glm::rotate(glm::translate(WTransform(), pc.position), pc.rotation) * m_world.pos_c(pc.parent).global_transform;
        }
        if (DragFloat("rotation", &pc.rotation)) {
            pc.global_transform = glm::rotate(glm::translate(WTransform(), pc.position), pc.rotation) * m_world.pos_c(pc.parent).global_transform;
        }
        int parent = pc.parent;
        if (InputInt("parent", &parent)) {
            pc.parent = (unsigned int) parent;
            pc.global_transform = glm::rotate(glm::translate(WTransform(), pc.position), pc.rotation) * m_world.pos_c(pc.parent).global_transform;
        }
    }

    // movement
    if (m_world.entities()[m_entity].test(CMove) and CollapsingHeader("movement")) {
        auto &mc = m_world.move_c(m_entity);
        float data[2] = {mc.velocity.x, mc.velocity.y};
        if (DragFloat2("velocity", data)) {
            mc.velocity = {data[0], data[1]};
        }
        data[0] = mc.accel.x; data[1] = mc.accel.y;
        if (DragFloat2("accel", data)) {
            mc.accel = {data[0], data[1]};
        }
        int movestate = static_cast<int>(mc.movestate);
        if (Combo("MoveState", &movestate, movestate_names.data(), movestate_names.size())) {
            mc.movestate = static_cast<MoveState>(movestate);
        }
        int moveset = static_cast<int>(mc.moveset);
        if (Combo("MoveSet", &moveset, moveset_names.data(), moveset_names.size())) {
            mc.moveset = static_cast<MoveSet>(moveset);
        }
        if (DragFloat("mass", &mc.mass)) {}
    }

    // static col
    if (m_world.entities()[m_entity].test(CStaticCol) and CollapsingHeader("static collision")) {
        auto &sc = m_world.static_col_c(m_entity);
        int response = static_cast<int>(sc.col_response);
        if (Combo("response", &response, col_responses.data(), col_responses.size())) {
            sc.col_response = static_cast<StaticColResponse>(response);
        }
    }
    // appearance
    // input
    if (m_world.entities()[m_entity].test(CInput) and CollapsingHeader("input")) {
        auto &ic = m_world.input_c(m_entity);
        int inputstate = static_cast<int>(ic.input_func);
        if (Combo("InputFunc", &inputstate, inputfunc_names.data(), inputfunc_names.size())) {
            ic.input_func = static_cast<InputFunc>(inputstate);
        }
    }
    // ground move
    if (m_world.entities()[m_entity].test(CGroundMove) and CollapsingHeader("ground movement")) {
        auto &gc = m_world.ground_move_c(m_entity);
        if (DragFloat("accel", &gc.c_accel)) {}
        if (DragFloat("stop_friction", &gc.c_stop_friction)) {}
        if (DragFloat("turn_mod", &gc.c_turn_mod)) {}
        if (DragFloat("max vel", &gc.c_max_vel)) {}
    }
    // jump
    if (m_world.entities()[m_entity].test(CJump) and CollapsingHeader("jumping")) {
        auto &jc = m_world.jump_c(m_entity);
        if (DragFloat("accel", &jc.c_accel)) {}
        if (DragFloat("jump height", &jc.c_jump_height)) {}
        if (DragFloat("turn_mod", &jc.c_turn_mod)) {}
        if (DragFloat("max vel", &jc.c_max_vel)) {}
    }
    // fly
    if (m_world.entities()[m_entity].test(CFly) and CollapsingHeader("flying")) {
        auto &fc = m_world.fly_c(m_entity);
        if (DragFloat("lift", &fc.c_lift, .0001f, 0.0f, 0.0f, "%.5f")) {}
        if (DragFloat("stall angle", &fc.c_stall_angle)) {}
        if (DragFloat("max angle change", &fc.c_max_change_angle)) {}
        if (DragFloat("accel force", &fc.c_accel_force)) {}
        if (DragFloat("accel time", &fc.c_accel_time)) {}
        if (DragFloat("drag", &fc.c_drag, .0001f, 0.0f, 0.0f, "%.5f")) {}
        if (DragFloat("push vel", &fc.c_push_vel)) {}
        ImVec2 points[4] = {{fc.from.x, fc.from.y}, {fc.ctrl_from.x, fc.ctrl_from.y}, {fc.ctrl_to.x, fc.ctrl_to.y}, {fc.to.x, fc.to.y}};
        if (Bezier("accel curve", points)) {
            fc.from = {points[0].x, points[0].y};
            fc.ctrl_from = {points[1].x, points[1].y};
            fc.ctrl_to = {points[2].x, points[2].y};
            fc.to = {points[3].x, points[3].y};
        }
    }
    // pathing
    if (m_world.entities()[m_entity].test(CPathing) and CollapsingHeader("pathing")) {
        auto &pc = m_world.path_c(m_entity);
        auto flags = pc.p_type.to_ulong();
        for (auto &pair : pathingtype_string) {
            CheckboxFlags(pair.second, &flags, 1 << static_cast<int>(pair.first));
        }
        pc.p_type = PathingType(flags);
        if (DragFloat("max mh dist", &pc.c_max_mh_dist)) {}
        if (DragFloat("padding", &pc.c_padding)) {}
        Text(std::to_string(pc.index).c_str());
    }
    // simplefly
    if (m_world.entities()[m_entity].test(CSimpleFly) and CollapsingHeader("simple fly")) {
        auto &fc = m_world.simple_fly_c(m_entity);
        if (DragFloat("max vel", &fc.c_max_vel)) {}
        if (DragFloat("accel", &fc.c_accel)) {}
        if (DragFloat("near threshold", &fc.c_near_threshold)) {}
        if (DragFloat("stop coeff", &fc.c_stop_coef)) {}
    }
    // skill
    // shape
    if (m_world.entities()[m_entity].test(CColShape) and CollapsingHeader("col shape")) {
        auto &shape = m_world.cshape_c(m_entity).shape;
        if (!shape) {
            shape = std::shared_ptr<ColShape>(
                    new ColCapsule(protagonist::c_capsule_size.x,
                                   protagonist::c_capsule_size.y));
        }
        int shape_type = static_cast<int>(shape->m_type);
        if (Combo("Shape", &shape_type, colshape_names)) {
            switch (static_cast<ColShapeName>(shape_type)) {
                case ColShapeName::ColCapsule: {
                    if (shape->m_type != ColShapeName::ColCapsule) {
                        shape = std::shared_ptr<ColShape>(
                                new ColCapsule(protagonist::c_capsule_size.x,
                                               protagonist::c_capsule_size.y));
                    }
                    break;
                }
                default: break;
            }
        }
        switch (shape->m_type) {
            case ColShapeName::ColCapsule: {
                auto cap_shape = dynamic_cast<ColCapsule*>(shape.get());
                float radius = cap_shape->get_capsule_radius();
                if (DragFloat("radius", &radius)) {
                    cap_shape->set_capsule_radius(radius);
                }
                float length = cap_shape->m_length;
                if (DragFloat("length", &length)) {
                    cap_shape->set_length(length);
                }
                break;
            }
            default: break;
        }
    }
    // dyn col
    if (m_world.entities()[m_entity].test(CDynCol) and CollapsingHeader("DynCol")) {
        auto &dc = m_world.dyn_col_c(m_entity);
        int response = static_cast<int>(dc.col_response);
        if (Combo("response", &response, col_responses.data(), col_responses.size())) {
            dc.col_response = static_cast<DynColResponse>(response);
        }
    }
    // tag

    if (Button("save entity")) {
        save_entity_standalone(m_world, m_entity);
    }
    SameLine();
    if (Button("delete entity")) {
        delete_entity_from_scene(m_world, m_entity);
        transition = EditorSubState(new EditorIdle);
    }
    SameLine();
    if (Button("move entity")) {
        transition = EditorSubState(new EntityMove(m_world, m_entity, m_mpos));
    }
    End();
    return transition;
}

EditorSubState EntityIdle::cancel() {
    return EditorSubState(new EditorIdle);
}

EditorSubState EntityIdle::move(GameWorld &) {
    return EditorSubState(new EntityMove(m_world, m_entity, m_mpos));
}

void EntityMove::draw(GameWorld &world) {
    bset debug_draw; debug_draw.set(CPosition); debug_draw.set(CDebugDraw);

    if (!for_gameworld::has_component(world.entities()[m_entity], debug_draw))  {
        return;
    }
    const auto &shape = world.cshape_c(m_entity).shape;
    auto circle = ColCircle(shape->get_radius());
    circle.m_highlight = true;
    circle.add_gfx_lines(world.pos_c(m_entity).global_transform);
}

EditorSubState EntityMove::cancel() {
    auto &pc = m_world.pos_c(m_entity);

    pc.position += m_diff;
    pc.global_transform = glm::rotate(glm::translate(WTransform(), pc.position), pc.rotation) * m_world.pos_c(pc.parent).global_transform;

    return EditorSubState(new EntityIdle(m_world, m_entity));
}

EntityMove::EntityMove(GameWorld &world, unsigned int entity, const WVec &mouse)
    : m_world(world), m_entity(entity) {
    m_diff = m_world.pos_c(m_entity).position - mouse;
    m_mpos = mouse;
    m_world.pos_c(m_entity).position = mouse;
}

EditorSubState EntityMove::update(const WVec &mpos) {
    auto &pc = m_world.pos_c(m_entity);

    auto diff = m_mpos - mpos;
    m_diff += diff;
    BaseEditorSubState::update(mpos);

    pc.position -= diff;
    pc.global_transform = glm::rotate(glm::translate(WTransform(), pc.position), pc.rotation) * m_world.pos_c(pc.parent).global_transform;

    return nullptr;
}

EditorSubState EntityMove::confirm(GameWorld &) {
    return EditorSubState(new EntityIdle(m_world, m_entity));
}
