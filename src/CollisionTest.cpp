//
// Created by tobi on 4/3/17.
//

#include "CollisionTest.h"
#include "Collision.h"

void CollisionTestState::update(Engine &engine) {
    if (!engine.get_focus()) {
        return;
    }
    const sf::RenderWindow &window = engine.get_window();
    auto mouse = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    WVec vel = (mouse - m_circle_tf.transformPoint(0, 0)) * 0.1f;
    m_circle_tf.translate(vel).rotate(33);

    for (auto &tri : m_grid.get_objects()) {
        tri->m_highlight = false;
    }
    //circle to world
    m_circle->transform(m_circle_tf);
    ColResult result;

    auto colliders = m_grid.find_colliders(m_circle);
    for (const auto &tri : colliders) {
        auto cr = m_circle->collides(*tri);
        if (cr.collides) {
            tri->m_highlight = true;
            if (cr.e > result.e) {
                result = cr;
            }
        }
    }
    // in this example it's okay to reset the transformation here.
    // this deals only with the environmental collision.
    // (what to do about moving platforms)
    m_circle->transform(m_circle_tf.getInverse());
    m_circle_tf.rotate(-33);
    if (result.collides) {
        auto move_back = find_directed_overlap(result, vel);
        m_circle_tf.translate(move_back);

        // slide movement and collide again
        vel = slide(-move_back, result.normal);
        m_circle_tf.translate(vel).rotate(33);
        //circle to world
        m_circle->transform(m_circle_tf);

        result = ColResult();
        for (const auto &tri : colliders) {
            auto cr = m_circle->collides(*tri);
            if (cr.collides) {
                tri->m_highlight = true;
                if (cr.e> result.e) {
                    result = cr;
                }
            }
        }
        m_circle->transform(m_circle_tf.getInverse());
        m_circle_tf.rotate(-33);
        if (result.collides) {
            move_back = find_directed_overlap(result, vel);
            m_circle_tf.translate(move_back);
        }
    }
}

CollisionTestState::CollisionTestState() {
    m_circle = std::shared_ptr<ColShape>(new ColCapsule(25, 50));
}

void CollisionTestState::draw(sf::RenderWindow &window) {
    sf::VertexArray lines_va(sf::Lines);
    WTransform zero_tf;
    for (const auto &tri : m_grid.get_objects()) {
        tri->add_gfx_lines(lines_va, zero_tf);
    }
    m_circle->add_gfx_lines(lines_va, m_circle_tf.rotate(33));
    m_circle_tf.rotate(-33);
    window.draw(lines_va);
}

void CollisionTestState::receive_tris(const std::vector<WVec> triangles) {
    m_grid.clear();
    for (uint i = 0 ; i < triangles.size() / 3 ; ++i) {
        m_grid.add_object(std::shared_ptr<ColShape>(new ColTriangle(triangles[i*3], triangles[i*3+1], triangles[i*3+2])));
    }
}

