#ifndef WBEHAVIOURTREEBUILDER_H
#define WBEHAVIOURTREEBUILDER_H

#include <memory>
#include <utility>

template <class parent_type, class behaviour_type>
class WLeafBuilder {
public:
    WLeafBuilder(parent_type& parent, std::shared_ptr<behaviour_type> leaf_behaviour) :
        m_parent(parent), m_leaf_behaviour(leaf_behaviour) {}

    template <typename ret, typename obj, typename... param_types>
    WLeafBuilder& invoke(ret (obj::*func)(param_types...), param_types... params) {
      (m_leaf_behaviour->*func)(params...);
      return (*this);
    }

    parent_type& end(void) {
      return m_parent;
    }

private:
    parent_type& m_parent; // this node's parent builder
    std::shared_ptr<behaviour_type> m_leaf_behaviour; // the raw behavior we're constructing
};

template <class parent_type, class decorator_type>
class WDecoratorBuilder;
template <class parent_type, class composite_type>
class WCompositeBuilder;

template <class parent_type, class composite_type>
class WCompositeBuilder {
    public:
    // constructor
    WCompositeBuilder(parent_type& parent, std::shared_ptr<composite_type> comp) :
        m_parent(parent), m_composite(comp) {}

    template <class node_type, typename... param_types>
        WLeafBuilder<WCompositeBuilder, node_type> node(param_types &&... params) {
            auto node = std::make_shared<node_type>(std::forward<param_types>(params)...);
            m_composite->add_child(node);
            return WLeafBuilder<WCompositeBuilder, node_type>(*this, node);
        }

    template <class other_composite_type, typename... param_types>
        WCompositeBuilder<WCompositeBuilder, other_composite_type> composite(param_types &&... params) {
            auto node = std::make_shared<other_composite_type>(std::forward<param_types>(params)...);
            m_composite->add_child(node);
            return WCompositeBuilder<WCompositeBuilder, other_composite_type>(*this, node);
        }

    template <class decorator_type, typename... param_types>
        WDecoratorBuilder<WCompositeBuilder, decorator_type> decorator(param_types &&... params) {
            auto node = std::make_shared<decorator_type>(std::forward<param_types>(params)...);
            m_composite->add_child(node);
            return WDecoratorBuilder<WCompositeBuilder, decorator_type>(*this, node);
        }

    parent_type& end(void) {
        return m_parent;
    }

    private:
    parent_type& m_parent;
    std::shared_ptr<composite_type> m_composite;
};


template <class parent_type, class decorator_type>
class WDecoratorBuilder {
public:
    WDecoratorBuilder(parent_type& parent, std::shared_ptr<decorator_type> decor) :
        m_parent(parent), m_decorator(decor) {}

    template <class node_type, typename... param_types>
        WLeafBuilder<WDecoratorBuilder, node_type> node(param_types &&... params) {
            auto node = std::make_shared<node_type>(std::forward<param_types>(params)...);
            m_decorator->set_child(node);
            return WLeafBuilder<WDecoratorBuilder, node_type>(*this, node);
        }

    template <class composite_type, typename... param_types>
    WCompositeBuilder<WDecoratorBuilder, composite_type> composite(param_types &&... params) {
            auto node = std::make_shared<composite_type>(std::forward<param_types>(params)...);
            m_decorator->set_child(node);
            return WCompositeBuilder<WDecoratorBuilder, composite_type>(*this, node);
    }

    template <class other_decorator_type, typename... param_types>
        WDecoratorBuilder<WDecoratorBuilder, other_decorator_type> decorator(param_types &&... params) {
            auto node = std::make_shared<other_decorator_type>(std::forward<param_types>(params)...);
            m_decorator->set_child(node);
            return WDecoratorBuilder<WDecoratorBuilder, other_decorator_type>(*this, node);
        }

    parent_type& end(void) {
        return m_parent;
    }

private:
    parent_type& m_parent;
    std::shared_ptr<decorator_type> m_decorator;
};

template <class selector_type>
class WBehaviourTreeBuilder;

template <class selector_type>
class WBehaviourTreeBuilder {
public:
    WBehaviourTreeBuilder() {
        m_root = std::make_shared<selector_type>();
    }

    template <class node_type, typename... param_types>
        WLeafBuilder<WBehaviourTreeBuilder, node_type> node(param_types &&... params) {
            auto node = std::make_shared<node_type>(std::forward<param_types>(params)...);
            m_root->add_child(node);
            return WLeafBuilder<WBehaviourTreeBuilder, node_type>(*this, node);
        }

    template <class composite_type, typename... param_types>
        WCompositeBuilder<WBehaviourTreeBuilder, composite_type> composite(param_types &&... params) {
            auto node = std::make_shared<composite_type>(std::forward<param_types>(params)...);
            m_root->add_child(node);
            return WCompositeBuilder<WBehaviourTreeBuilder, composite_type>(*this, node);
        }

    template <class decorator_type, typename... param_types>
        WDecoratorBuilder<WBehaviourTreeBuilder, decorator_type> decorator(param_types &&... params) {
            auto node = std::make_shared<decorator_type>(std::forward<param_types>(params)...);
            m_root->add_child(node);
            return WDecoratorBuilder<WBehaviourTreeBuilder, decorator_type>(*this, node);
        }

    selector_type end(void) {
        return *m_root;
    }

private:
    std::shared_ptr<selector_type> m_root;
};

#endif /* WBEHAVIOURTREEBUILDER_H */
