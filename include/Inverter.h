#ifndef INVERTER_H
#define INVERTER_H

#include "WBehaviourTree.h"

class Inverter : public behaviour_tree::Decorator {
    public:
        behaviour_tree::Status update() override {
            using namespace behaviour_tree;
            auto status = m_child->tick();

            if (status == Status::Failure) {
                return Status::Success;
            }
            assert(status == Status::Success);
            return Status::Failure;
        }
};


#endif /* INVERTER_H */
