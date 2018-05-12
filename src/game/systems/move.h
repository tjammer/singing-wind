#ifndef MOVE_H
#define MOVE_H

struct Movement;
struct Transform;

void
move_update(Movement& mc, Transform& pc, float dt);

void
build_global_transform(Transform& pc);

#endif /* MOVE_H */
