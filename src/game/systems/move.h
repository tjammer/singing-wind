#ifndef MOVE_H
#define MOVE_H

struct Movement;
struct Position;

void
move_update(Movement& mc, Position& pc, float dt);

void
build_global_transform(Position& pc);

#endif /* MOVE_H */
