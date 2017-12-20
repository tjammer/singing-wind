#ifndef WBEHAVIOURTREE_H
#define WBEHAVIOURTREE_H

#include <memory>
#include <array>
#include <assert.h>

namespace behaviour_tree {
#include "WBehaviourTreeBuilder.h"

const int c_composite_max_children = 5;

enum class Status {
    Success,
    Failure,
    Running,
    NotInit
};

class Behaviour {
public:
    Behaviour() : m_status(Status::NotInit) {}

    virtual Status update() = 0;
    virtual void enter() {}
    virtual void terminate(Status) {}

    Status tick() {
        if (m_status != Status::Running) {
            enter();
        }
        m_status = update();
        if (m_status != Status::Running) {
            terminate(m_status);
        }
        return m_status;
    }

    void reset() {
        m_status = Status::NotInit;
    }

    Status status() const {
        return m_status;
    }

private:
    Status m_status;
};

class Decorator : public Behaviour {
public:
    void set_child(std::shared_ptr<Behaviour> node) {
        m_child = node;
    }

protected:
    std::shared_ptr<Behaviour> m_child;
};

class Composite : public Behaviour {
public:
    void add_child(std::shared_ptr<Behaviour> node) {
        assert(m_nchildren < c_composite_max_children);
        m_children[m_nchildren] = node;
        m_nchildren++;
    }

protected:
    std::array<std::shared_ptr<Behaviour>, c_composite_max_children> m_children;
    size_t m_nchildren = 0;
};

class Sequence : public Composite {
public:
    void enter() override {
        m_current = 0;
    }

    Status update() override {
        while (m_current < m_nchildren) {
            auto status = m_children[m_current]->tick();
            if (status != Status::Success) {
                return status;
            }
            m_current++;
        }
        return Status::Success;
    }

private:
    size_t m_current;
};

class Selector : public Composite {
public:
    void enter() override {
        m_current = 0;
    }

    Status update() override {
        while (m_current < m_nchildren) {
            auto status = m_children[m_current]->tick();
            if (status != Status::Failure) {
                return status;
            }
            m_current++;
        }
        return Status::Failure;
    }

private:
    size_t m_current;
};

using BehaviourTreeBuilder = WBehaviourTreeBuilder<Selector>;

}


#endif /* WBEHAVIOURTREE_H */
