#ifndef ACTOR_H
#define ACTOR_H

class GameWorld;

namespace actor {
    void on_static_collision(GameWorld &world, unsigned int entity);
    void on_dynamic_collision(GameWorld&, unsigned int entity);
}


#endif /* ACTOR_H */
