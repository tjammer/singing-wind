#ifndef RUNFORSECOND_H
#define RUNFORSECOND_H

#include "WBehaviourTree.h"

class RunForFrames : public behaviour_tree::Decorator
{
public:
  RunForFrames(unsigned int n)
    : m_maxn(n)
  {
  }
  behaviour_tree::Status update() override;
  void enter() override { m_current = 0; }

private:
  unsigned int m_maxn;
  unsigned int m_current{ 0 };
};

#endif /* RUNFORSECOND_H */
