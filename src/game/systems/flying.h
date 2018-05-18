#ifndef FLYING_H
#define FLYING_H

struct Transform;
struct Input;
struct Movement;

// void
// flying(CanFly,
//       const Flying& fc,
//       Movement& mc,
//       const Position& pc,
//       const Input& ic);

void
dummy_flying(const Transform& pc, Movement& mc, const Input& ic);

void
hover(const Transform&, Movement&, const Input&);

#endif /* FLYING_H */
