//
// Created by tobi on 3/31/17.
//

#include "EditorStates.h"
#include <iostream>
#include <imgui.h>

std::unique_ptr<BaseEditorSubState> IslandIdle::confirm(Island &island) {
    // find curve nearest to cursor
    auto dist = std::numeric_limits<float>::max();
    auto size = island.m_points.size();
    uint curve_index = 0;
    for (uint i = 0; i < size; ++i) {
        BCurve curve = BCurve{island.m_points[i], island.m_ctrl_points[i*2],
                              island.m_ctrl_points[i*2 +1], island.m_points[(i+1)%size]};
        auto vecs = curve.points_along_curve(1.0f);
        for (auto v : vecs) {
            if (w_magnitude(v - m_mpos) < dist) {
                dist = w_magnitude(v - m_mpos);
                curve_index = i;
            }
        }
    }
    if (dist > 50) {
        return std::unique_ptr<IslandIdle>(new IslandIdle);
    }
    auto curve = BCurve{island.m_points[curve_index], island.m_ctrl_points[curve_index*2],
                        island.m_ctrl_points[curve_index*2 +1], island.m_points[(curve_index+1)%size]};
    return std::unique_ptr<CurveIdle>(new CurveIdle(curve));
}

EditorSubState IslandIdle::menu(Island &island) {
    ImGui::Begin("island idle");
    if (ImGui::Button("select curve nearest to cursor")) {
        ImGui::End();
        return confirm(island);
    }
    if (ImGui::Button("cancel")) {
        ImGui::End();
        return cancel();
    }
    ImGui::End();
    return nullptr;
}

CurveIdle::CurveIdle(const BCurve &curve) : m_curve(curve) {
}

void CurveIdle::draw(sf::RenderWindow &window) {
    sf::VertexArray va(sf::Quads);
    sf::VertexArray lines(sf::Lines);
    for (auto v : m_curve.line_along_curve(line_draw_distance)) {
        va.append(sf::Vertex(v, sf::Color(128, 8, 128)));
    }

    for (const auto& point : {m_curve.from, m_curve.ctrl_from, m_curve.ctrl_to, m_curve.to}) {
        for (const auto& v : make_quad(point, ctrl_point_size)) {
            va.append(sf::Vertex(v, sf::Color(255, 255, 255)));
        }
    }

    lines.append(sf::Vertex(m_curve.from, sf::Color(255, 255, 255)));
    lines.append(sf::Vertex(m_curve.ctrl_from, sf::Color(255, 255, 255)));
    lines.append(sf::Vertex(m_curve.ctrl_to, sf::Color(255, 255, 255)));
    lines.append(sf::Vertex(m_curve.to, sf::Color(255, 255, 255)));

    window.draw(lines);
    window.draw(va);
}

EditorSubState CurveIdle::confirm(Island &island) {
    auto dist = std::numeric_limits<float>::max();
    uint point_index = 0;
    for (uint i = 0 ; i < 4 ; ++i) {
        if (w_magnitude(m_curve[i] - m_mpos) < dist) {
            dist = w_magnitude(m_curve[i] - m_mpos);
            point_index = i;
        }
    }

    if (dist > 70) {
        return std::unique_ptr<IslandIdle>(new IslandIdle);
    }

    auto it = std::find(island.m_points.begin(), island.m_points.end(), m_curve[point_index]);
    if (it == island.m_points.end()) {
        it = std::find(island.m_ctrl_points.begin(), island.m_ctrl_points.end(), m_curve[point_index]);
    }

    auto at = std::find(island.m_points.begin(), island.m_points.end(), *it);
    if (at != island.m_points.end()) {
        auto i = at - island.m_points.begin();
        auto& c1 = island.m_ctrl_points[i*2];
        auto& c2 = island.m_ctrl_points[(i*2-1)%island.m_ctrl_points.size()];
        return EditorSubState(new PointEdit(*it, m_mpos, c1, c2));
    }
    return EditorSubState(new PointEdit(*it, m_mpos));
}

EditorSubState CurveIdle::insert_item(Island &island) {
    return EditorSubState(new CurveInsert(m_curve));
}

EditorSubState CurveIdle::menu(Island &island) {
    ImGui::Begin("curve idle");
    if (ImGui::Button("add point")) {
        ImGui::End();
        return insert_item(island);
    }
    if (ImGui::Button("cancel")) {
        ImGui::End();
        return cancel();
    }
    ImGui::End();
    return nullptr;
}



void PointEdit::update(const WVec &mpos) {
    auto diff = m_mpos - mpos;
    BaseEditorSubState::update(mpos);
    m_point -= diff;
    if (!ctrl) {
        m_c1 -= diff;
        m_c2 -= diff;
    }
}

EditorSubState PointEdit::cancel() {
    auto diff = m_point - m_orig_point;
    m_point = m_orig_point;
    if (!ctrl) {
        m_c1 -= diff;
        m_c2 -= diff;
    }
    return EditorSubState(new IslandIdle);
}

void PointEdit::draw(sf::RenderWindow &window) {
    sf::VertexArray va(sf::Quads);
    for (const auto &v : make_quad(m_point, point_size)) {
        va.append(sf::Vertex(v, sf::Color(128, 8, 128)));
    }
    window.draw(va);
}

PointEdit::PointEdit(WVec &point, const WVec &mouse, WVec &c1, WVec &c2) : m_point(point), m_c1(c1), m_c2(c2) {
    m_orig_point = m_point;
    m_mpos = mouse;
}

PointEdit::PointEdit(WVec &point, const WVec &mouse) : m_point(point), m_c1(point), m_c2(point){
    ctrl = true;
    m_orig_point = m_point;
    m_mpos = mouse;
}

EditorSubState PointEdit::menu(Island &island) {
    ImGui::Begin("point move");
    if (ImGui::Button("place point here")) {
        ImGui::End();
        return confirm(island);
    }
    if (ImGui::Button("delete point")) {
        ImGui::End();
        return delete_item(island);
    }
    if (ImGui::Button("cancel")) {
        ImGui::End();
        return cancel();
    }
    ImGui::End();
    return nullptr;
}

EditorSubState PointEdit::delete_item(Island &island) {
    auto size = island.m_points.size();
    if (island.m_points.size() == 4) {
        return EditorSubState(new IslandIdle);
    }
    auto i = static_cast<size_t>(std::find(island.m_points.begin(), island.m_points.end(), m_point) - island.m_points.begin());
    // check if this is a control point
    if (i == size) {
        auto it = std::find(island.m_ctrl_points.begin(), island.m_ctrl_points.end(), m_point);
        if (it != island.m_ctrl_points.end()) {
            return EditorSubState(new IslandIdle);
        }
    }

    auto curve = BCurve{island.m_points[(i-1)%size],
                        island.m_ctrl_points[((i-1)%size)*2],
                        island.m_ctrl_points[((i-1)%size)*2 +1],
                        island.m_points[i]};
    auto it = std::find(island.m_points.begin() + 1, island.m_points.end(), curve.to);
    island.m_points.erase(it);

    it = std::find(island.m_ctrl_points.begin(), island.m_ctrl_points.end(), curve.ctrl_to);
    island.m_ctrl_points.erase(it, it+2);

    return EditorSubState(new IslandIdle);
}

void CurveInsert::draw(sf::RenderWindow &window) {
    sf::VertexArray va(sf::Quads);

    float t_low = fmax(m_new_point_t - 0.01f, 0.f);
    float t_high = fmin(m_new_point_t + 0.01f, 1.f);

    auto base = m_curve.eval(t_low);
    va.append(sf::Vertex(base + m_curve.eval_perpendicular(t_low) * line_size, sf::Color(128, 8, 128)));
    va.append(sf::Vertex(base - m_curve.eval_perpendicular(t_low) * line_size, sf::Color(128, 8, 128)));
    base = m_curve.eval(t_high);
    va.append(sf::Vertex(base - m_curve.eval_perpendicular(t_high) * line_size, sf::Color(128, 8, 128)));
    va.append(sf::Vertex(base + m_curve.eval_perpendicular(t_high) * line_size, sf::Color(128, 8, 128)));
    window.draw(va);
}

CurveInsert::CurveInsert(BCurve curve) : m_curve(curve) {
}

void CurveInsert::update(const WVec &mpos) {
    BaseEditorSubState::update(mpos);
    auto to = (m_mpos - m_curve.to);
    auto from = (m_mpos - m_curve.from);

    if (to.x * from.x + to.y * from.y >= 0) {
        m_new_point_t = w_magnitude(to) > w_magnitude(from) ? 0 : 1;
    }
    else {
        m_new_point_t = 1.f - w_magnitude(to) / (w_magnitude(from) + w_magnitude(to));
    }
}

EditorSubState CurveInsert::confirm(Island &island) {
    auto new_point = m_curve.eval(m_new_point_t);

    auto it = std::find(island.m_ctrl_points.begin(), island.m_ctrl_points.end(), m_curve.ctrl_to);
    it = island.m_ctrl_points.insert(it, new_point + (m_curve.from - new_point) / 4.f);
    island.m_ctrl_points.insert(it + 1, new_point - (new_point - m_curve.to) / 4.f);
    *(it + 2) = new_point - (new_point - m_curve.to) * 0.75f;
    *(it - 1) = new_point + (m_curve.from - new_point) * 0.75f;

    it = std::find(island.m_points.begin() + 1, island.m_points.end(), m_curve.to);
    island.m_points.insert(it, new_point);

    return EditorSubState(new IslandIdle);
}

EditorSubState CurveInsert::menu(Island &island) {
    ImGui::Begin("curve insert");
    if (ImGui::Button("insert curve here")) {
        ImGui::End();
        return confirm(island);
    }
    if (ImGui::Button("cancel")) {
        ImGui::End();
        return cancel();
    }
    ImGui::End();
    return nullptr;
}
