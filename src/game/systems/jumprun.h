#ifndef JUMPRUN_H
#define JUMPRUN_H

struct Movement;
struct Input;
struct Transform;

void
jump_run_update(Movement& mc, Transform& t, const Input& ic);

#endif /* JUMPRUN_H */
