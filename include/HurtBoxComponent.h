#ifndef HURTBOXCOMPONENT_H
#define HURTBOXCOMPONENT_H

#include <functional>
#include <vector>

class GameWorld;

struct HurtBoxComponent {
    unsigned int owner = 0;
    std::vector<unsigned int> hit_entities;
    std::function<void(GameWorld &, const unsigned int)> hit_function = nullptr;
};

namespace hurtbox {
    void on_dynamic_collision(GameWorld &world, const unsigned int entity);
};


#endif /* HURTBOXCOMPONENT_H */
