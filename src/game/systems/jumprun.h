#ifndef JUMPRUN_H
#define JUMPRUN_H

struct Movement;
struct Input;
struct Transform;
struct JumpRun;

void
jump_run_update(JumpRun& jr, Movement& mc, Transform& t, const Input& ic);

#endif /* JUMPRUN_H */
