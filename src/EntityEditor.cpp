//
// Created by tobi on 4/26/17.
//

#include "EntityEditor.h"
#include "EditorStates.h"
#include "imgui.h"

const char* const moveset_names = {"Protagonist\0\0"};
const char* const movestate_names = {"OnGround\0Jumping\0Falling\0Flying\0FlyingAccel\0\0"};
const std::unordered_map<Components, std::string> component_names = {
        {CPosition, "Position"},
        {CMove, "Move"},
        {CStaticCol, "StaticCollision"},
        {CAppearance, "Appearance"},
        {CInput, "Input"},
        {CDebugDraw, "DebugDraw"},
        {CGroundMove, "GroundMove"},
        {CJump, "Jump"},
        {CFly, "Fly"}
};
const char* const colshape_names = {"Triangle\0Circle\0Capsule\0\0"};
const char* const col_responses = {"Actor\0\0"};

EditorSubState EntityIdle::cancel() {
    return EditorSubState(new EditorIdle);
}

EditorSubState EntityIdle::move(GameWorld &world) {
    return EditorSubState(new EntityIdle(world, m_entity));
}

EditorSubState EntityIdle::insert_item(GameWorld &world) {
    return EditorSubState(new EntityIdle(world, m_entity));
}

EditorSubState EntityIdle::delete_item(GameWorld &world) {
    return EditorSubState(new EntityIdle(world, m_entity));
}

EditorSubState EntityIdle::menu(GameWorld &world) {
    return EditorSubState(new EntityIdle(world, m_entity));
}

void EntityIdle::draw(GameWorld &world, sf::RenderWindow &window) {
    bset debug_draw; debug_draw.set(CPosition); debug_draw.set(CDebugDraw);

    if (!for_gameworld::has_component(world.m_entities[m_entity], debug_draw))  {
        return;
    }
    const auto &shape = world.m_debug_c[m_entity].shape;
    auto circle = ColCircle(shape->get_radius());
    circle.m_highlight = true;
    auto va = sf::VertexArray(sf::Lines);
    circle.add_gfx_lines(va, world.m_pos_c[m_entity].global_transform);
    window.draw(va);
}

EditorSubState EntityIdle::confirm(GameWorld &world) {
    return EditorSubState(new EntityIdle(world, m_entity));
}

void EntityIdle::update(const WVec &mpos) {
    using namespace std;
    using namespace ImGui;
    BaseEditorSubState::update(mpos);


    Begin("Entity Editor");
    // base information
    Text("id:");
    SameLine(100);
    Text(to_string(m_entity).c_str());

    Text("name:");
    SameLine(100);
    string &_name = m_world.m_id_c[m_entity];
    vector<char> entity_name(_name.begin(), _name.end());
    entity_name.push_back('\0');
    InputText("", &entity_name[0], entity_name.size());
    _name = string(entity_name.begin(), entity_name.end());

    // bitset
    if (CollapsingHeader("components bitset")) {
        auto &comps = m_world.m_entities[m_entity];

        auto flags = comps.to_ulong();
        CheckboxFlags(component_names.at(CMove).c_str(), &flags, 1 << CMove);
        CheckboxFlags(component_names.at(CAppearance).c_str(), &flags, 1 << CAppearance);
        CheckboxFlags(component_names.at(CStaticCol).c_str(), &flags, 1 << CStaticCol);
        CheckboxFlags(component_names.at(CInput).c_str(), &flags, 1 << CInput);
        CheckboxFlags(component_names.at(CDebugDraw).c_str(), &flags, 1 << CDebugDraw);
        CheckboxFlags(component_names.at(CGroundMove).c_str(), &flags, 1 << CGroundMove);
        CheckboxFlags(component_names.at(CJump).c_str(), &flags, 1 << CJump);
        CheckboxFlags(component_names.at(CFly).c_str(), &flags, 1 << CFly);
        comps = {flags};
        Text(comps.to_string().c_str());
    }

    // position
    if (m_world.m_entities[m_entity].test(CPosition) and CollapsingHeader("position")) {
        auto &pc = m_world.m_pos_c[m_entity];
        float data[2] = {pc.position.x, pc.position.y};
        if (DragFloat2("position", data)) {
            pc.position.x = data[0];
            pc.position.y = data[1];
            pc.global_transform = WTransform().combine(m_world.m_pos_c[pc.parent].global_transform).translate(
                    pc.position).rotate(pc.rotation);
        }
        if (DragFloat("rotation", &pc.rotation)) {
            pc.global_transform = WTransform().combine(m_world.m_pos_c[pc.parent].global_transform).translate(
                    pc.position).rotate(pc.rotation);
        }
        int parent = pc.parent;
        if (InputInt("parent", &parent)) {
            pc.parent = (unsigned int) parent;
            pc.global_transform = WTransform().combine(m_world.m_pos_c[pc.parent].global_transform).translate(
                    pc.position).rotate(pc.rotation);
        }
    }

    // movement
    if (m_world.m_entities[m_entity].test(CMove) and CollapsingHeader("movement")) {
        auto &mc = m_world.m_move_c[m_entity];
        float data[2] = {mc.velocity.x, mc.velocity.y};
        if (DragFloat2("velocity", data)) {
            mc.velocity = {data[0], data[1]};
        }
        data[0] = mc.accel.x; data[1] = mc.accel.y;
        if (DragFloat2("accel", data)) {
            mc.accel = {data[0], data[1]};
        }
        int movestate = static_cast<int>(mc.movestate);
        if (Combo("MoveState", &movestate, movestate_names)) {
            mc.movestate = static_cast<MoveState>(movestate);
        }
        int moveset = static_cast<int>(mc.moveset);
        if (Combo("MoveSet", &moveset, moveset_names)) {
            mc.moveset = static_cast<MoveSet>(moveset);
        }
    }

    // static col
    if (m_world.m_entities[m_entity].test(CStaticCol) and CollapsingHeader("static collision")) {
        auto &sc = m_world.m_static_col_c[m_entity];
        int shape_type = static_cast<int>(sc.shape->m_type);
        if (Combo("Shape", &shape_type, colshape_names)) {
            switch (static_cast<ColShapeName>(shape_type)) {
                case ColShapeName::ColCapsule: {
                    if (sc.shape->m_type != ColShapeName::ColCapsule) {
                        sc.shape = std::shared_ptr<ColShape>(new ColCapsule(15, 15));
                    }
                    break;
                }
                default: break;
            }
        }
        switch (sc.shape->m_type) {
            case ColShapeName::ColCapsule: {
                auto shape = dynamic_cast<ColCapsule*>(sc.shape.get());
                float radius = shape->m_capsule_radius;
                if (DragFloat("radius", &radius)) {
                    shape->m_capsule_radius = radius;
                }
                float data[2] = {shape->m_a.x, shape->m_a.y};
                if (DragFloat2("a", data)) {
                    shape->m_a = {data[0], data[1]};
                }
                data[0] = shape->m_b.x; data[1] = shape->m_b.y;
                if (DragFloat2("b", data)) {
                    shape->m_b = {data[0], data[1]};
                }
                break;
            }
            default: break;
        }
        int response = static_cast<int>(sc.col_response);
        if (Combo("response", &response, col_responses)) {
            sc.col_response = static_cast<StaticColResponse>(response);
        }
    }
    // appearance
    // input
    // ground move
    if (m_world.m_entities[m_entity].test(CGroundMove) and CollapsingHeader("ground movement")) {
        auto &gc = m_world.m_ground_move_c[m_entity];
        if (DragFloat("accel", &gc.c_accel)) {}
        if (DragFloat("stop_friction", &gc.c_stop_friction)) {}
        if (DragFloat("turn_mod", &gc.c_turn_mod)) {}
    }
    // jump
    if (m_world.m_entities[m_entity].test(CJump) and CollapsingHeader("jumping")) {
        auto &gc = m_world.m_jump_c[m_entity];
        if (DragFloat("accel", &gc.c_accel)) {}
        if (DragFloat("jump speed", &gc.c_jump_speed)) {}
        if (DragFloat("turn_mod", &gc.c_turn_mod)) {}
    }
    // fly
    if (m_world.m_entities[m_entity].test(CFly) and CollapsingHeader("flying")) {
        auto &gc = m_world.m_fly_c[m_entity];
        if (DragFloat("lift", &gc.c_lift)) {}
        if (DragFloat("stall angle", &gc.c_stall_angle)) {}
        if (DragFloat("max angle change", &gc.c_max_change_angle)) {}
        if (DragFloat("accel force", &gc.c_accel_force)) {}
        if (DragFloat("accel time", &gc.c_accel_time)) {}
    }
    End();
}
