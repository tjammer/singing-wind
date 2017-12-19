    //
    // Created by jammer on 11/04/17.
    //

#ifndef SINGING_WIND_PROTAGONIST_H
#define SINGING_WIND_PROTAGONIST_H

#include "MoveSystems.h"

class GameWorld;

namespace protagonist {
    void handle_inputs(GameWorld &world, unsigned int entity);
    const WVec c_capsule_size = {15, 30};
}

class GroundMove : public MoveState {
public:
    void enter(GameWorld &, unsigned int) override;
    void accel(GameWorld &, unsigned int) override;
    MoveStateName name() override;

    static bool transition(GameWorld &, unsigned int);
};


class FallingMove : public MoveState {
public:
    void enter(GameWorld &, unsigned int) override;
    void accel(GameWorld &, unsigned int) override;
    MoveStateName name() override;

    static bool transition(GameWorld &, unsigned int);
};

class FlyingMove: public MoveState {
public:
    void enter(GameWorld &, unsigned int) override;
    void accel(GameWorld &, unsigned int) override;
    MoveStateName name() override;

    static bool transition(GameWorld &, unsigned int);
};

class FlyingAccelMove : public MoveState {
public:
    void enter(GameWorld &, unsigned int) override;
    void accel(GameWorld &, unsigned int) override;
    MoveStateName name() override;

    static bool transition(GameWorld &, unsigned int);
};

class ProtagonistMoveSet : public MoveSet {
public:
    std::unique_ptr<MoveState> transition(GameWorld &, unsigned int) override;
    void init(GameWorld &, unsigned int) override;
    MoveSetName name() override {return MoveSetName::Protagonist;}
private:
    std::unique_ptr<MoveState> from_undefined(GameWorld &, unsigned int);
};

#endif //SINGING_WIND_PROTAGONIST_H
