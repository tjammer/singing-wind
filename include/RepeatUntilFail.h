#ifndef REPEATUNTILFAIL_H
#define REPEATUNTILFAIL_H

#include "WBehaviourTree.h"

class RepeatUntilFail : public behaviour_tree::Decorator
{
public:
  behaviour_tree::Status update() override
  {
    using namespace behaviour_tree;
    auto status = m_child->tick();

    if (status != Status::Failure) {
      return Status::Running;
    }
    return status;
  }
};

#endif /* REPEATUNTILFAIL_H */
