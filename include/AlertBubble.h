#ifndef ALERTBUBBLE_H
#define ALERTBUBBLE_H

class GameWorld;

namespace alert_bubble {
    void spawn(GameWorld &, unsigned int);
    void on_dynamic_collision(GameWorld &, unsigned int);
}


#endif /* ALERTBUBBLE_H */
