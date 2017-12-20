#include "NTimes.h"

behaviour_tree::Status NTimes::update() {
    using namespace behaviour_tree;
    auto status = m_child->tick();

    if (status != Status::Success) {
        return status;
    }

    if (++m_current < m_maxn) {
        return Status::Running;
    }

    return Status::Success;
}
