#ifndef MOVE_H
#define MOVE_H

struct Movement;
struct Position;

void
move_update(Movement& mc, Position& pc, float dt);

#endif /* MOVE_H */
