#include "RunForFrames.h"

behaviour_tree::Status
RunForFrames::update()
{
  using namespace behaviour_tree;
  auto status = m_child->tick();

  if (status != Status::Running) {
    return status;
  }

  if (++m_current < m_maxn) {
    return Status::Running;
  }
  return Status::Success;
}
