#ifndef NTIMES_H
#define NTIMES_H

#include "WBehaviourTree.h"

class NTimes : public behaviour_tree::Decorator
{
public:
  NTimes(unsigned int n)
    : m_maxn(n)
  {
  }
  behaviour_tree::Status update() override;
  void enter() override { m_current = 0; }

private:
  unsigned int m_maxn;
  unsigned int m_current = 0;
};

#endif /* NTIMES_H */
