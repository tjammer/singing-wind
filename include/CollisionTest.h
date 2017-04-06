//
// Created by tobi on 4/3/17.
//

#ifndef SINGING_WIND_COLLISIONTEST_H
#define SINGING_WIND_COLLISIONTEST_H

#include "Engine.h"
#include "ColShape.h"
#include "ColGrid.h"

class CollisionTestState : public EngineState {
public:
    void cleanup() override {};

    void update(Engine& engine) override;
    void draw(sf::RenderWindow& window);

    CollisionTestState();
    ~CollisionTestState() = default;

    void receive_tris(const std::vector<WVec> triangles);

private:
    std::shared_ptr<ColShape> m_circle;
    WTransform m_circle_tf;
    StaticGrid m_grid;
};


#endif //SINGING_WIND_COLLISIONTEST_H
