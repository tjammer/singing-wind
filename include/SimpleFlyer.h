#ifndef TESTENEMY_H
#define TESTENEMY_H

class GameWorld;

namespace simpleflyer {
    void on_static_collision(GameWorld &world, unsigned int entity);

    void to_simple_flying(GameWorld &world, unsigned int entity);
    void simple_flying(GameWorld &world, unsigned int entity);
    bool transition_simple_flying(GameWorld &world, unsigned int entity);

    void to_hover(GameWorld &, unsigned int);
    void hover(GameWorld &, unsigned int);
    bool transition_hover(GameWorld &, unsigned int);
}

#endif /* TESTENEMY_H */
