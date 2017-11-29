#ifndef LOUNGE_H
#define LOUNGE_H

#include "SkillComponent.h"
#include "MoveSystems.h"

class LoungeAttackMove : public TimedMoveState {
public:
    void enter(GameWorld &, unsigned int) override;
    void accel(GameWorld &, unsigned int) override;
    void leave(GameWorld &, unsigned int) override {}
    std::unique_ptr<TimedMoveState> next() override {return nullptr;}
    LoungeAttackMove() : TimedMoveState(TimedMoveStateName::LoungeAttack, 1.6) {}
};

class LoungeCastMove : public TimedMoveState {
public:
    void enter(GameWorld &, unsigned int) override {}
    void accel(GameWorld &, unsigned int) override;
    void leave(GameWorld &, unsigned int) override {}
    std::unique_ptr<TimedMoveState> next() override {return std::make_unique<LoungeAttackMove>();}
    LoungeCastMove() : TimedMoveState(TimedMoveStateName::LoungeCast, 1.2) {}
};

class LoungeSkill : public BaseSkill {
public:
    void set_special(GameWorld &, unsigned int) override;
    LoungeSkill() : BaseSkill(SkillID::Lounge, 2.8, 5) {}
};
#endif /* LOUNGE_H */
