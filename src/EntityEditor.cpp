//
// Created by tobi on 4/26/17.
//

#include "EntityEditor.h"
#include "EditorStates.h"
#include "imgui.h"

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
    if (CollapsingHeader("position")) {
        auto &pc = m_world.m_pos_c[m_entity];
        float data[2] = {pc.position.x, pc.position.y};
        if (InputFloat2("position", data)) {
            pc.position.x = data[0];
            pc.position.y = data[1];
            pc.global_transform = WTransform().combine(m_world.m_pos_c[pc.parent].global_transform).translate(pc.position).rotate(pc.rotation);
        }
        if (InputFloat("rotation", &pc.rotation)) {
            pc.global_transform = WTransform().combine(m_world.m_pos_c[pc.parent].global_transform).translate(pc.position).rotate(pc.rotation);
        }
        int parent = pc.parent;
        if (InputInt("parent", &parent)) {
            pc.parent = (unsigned  int)parent;
            pc.global_transform = WTransform().combine(m_world.m_pos_c[pc.parent].global_transform).translate(pc.position).rotate(pc.rotation);
        }

        // Movement
    }
    End();
}
