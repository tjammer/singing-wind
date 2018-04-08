#ifndef ECS_HPP
#define ECS_HPP

#include <bitset>
#include <vector>
#include <memory>
#include <assert.h>

namespace ecs {
using type_ct = std::size_t;

type_ct
get_next_type_ct()
{
  static type_ct ct = 0;
  return ++ct;
}

template<typename T>
struct comp_type
{
  static const type_ct val;
};

template<typename T>
const type_ct comp_type<T>::val = get_next_type_ct();

template<typename T>
type_ct
get_type_ct()
{
  return comp_type<T>::val;
}

const size_t comps_limit = 32;
struct entity
{
  std::bitset<comps_limit> components = 0;
};

// find first type
template<typename T, typename... Ts>
struct first
{
  using type = T;
};

template<typename... Ts>
using first_t = typename first<Ts...>::type;

template<typename T>
struct primary
{
  using type = T;
};

class component_set
{
public:
  using size_type = std::size_t;
  virtual ~component_set() = 0;
  virtual void remove(size_type ent) = 0;
};

inline component_set::~component_set() = default;

template<typename T>
class component_set_impl : public component_set
{
public:
  virtual ~component_set_impl() = default;

  size_type assign(size_type entid, T com)
  {
    if (entid >= entid_to_comid.size()) {
      entid_to_comid.resize(entid + 1, -1);
    }

    auto comid = comid_to_entid.size();

    entid_to_comid[entid] = comid;
    comid_to_entid.push_back(entid);
    components.push_back(std::move(com));

    return comid;
  }

  virtual void remove(size_type entid) override
  {
    auto last = comid_to_entid.size() - 1;
    auto comid = entid_to_comid[entid];

    entid_to_comid[entid] = -1;
    entid_to_comid[comid_to_entid[last]] = comid;

    comid_to_entid[comid] = comid_to_entid[last];
    comid_to_entid.pop_back();

    components[comid] = std::move(components[last]);
    components.pop_back();
  }
  size_type get_comid(size_type entid) const { return entid_to_comid[entid]; }

  T& get_com(size_type comid) { return components[comid]; }

  size_type get_entid(size_type comid) const { return comid_to_entid[comid]; }

  size_type size() const { return components.size(); }

private:
  std::vector<size_type> entid_to_comid;
  std::vector<size_type> comid_to_entid;
  std::vector<T> components;
};

template<typename T>
struct tag
{};

template<typename T>
class component_set_impl<tag<T>> : public component_set
{
public:
  virtual ~component_set_impl() = default;
  virtual void remove(size_type) override {}
};

namespace component_kinds {
struct std
{};
struct normal : public std
{};
struct tagged : public std
{};
struct meta
{};
struct world : public meta
{};
struct ent : public meta
{};
}

template<typename World, typename Component>
struct component_kind
{
  using category = component_kinds::normal;
  using component = Component;
};

template<typename World>
struct component_kind<World, typename World::ent_id>
{
  using category = component_kinds::ent;
  using component = void;
};

template<typename World>
struct component_kind<World, World>
{
  using category = component_kinds::world;
  using component = void;
};

template<typename World, typename Component>
struct component_kind<World, tag<Component>>
{
  using category = component_kinds::tagged;
  using component = Component;
};

template<typename World>
struct world_traits
{
  using ent_id = typename World::ent_id;
  using com_id = typename World::com_id;

  template<typename comp>
  using comp_kind = component_kind<World, comp>;

  // primary forward decl
  template<typename HeadTag, typename... Components>
  struct get_primary;

  template<typename... Components>
  using get_primary_t =
    typename get_primary<typename comp_kind<first_t<Components...>>::category,
                         Components...>::type;

  template<typename HeadTag, typename HeadCom, typename... Components>
  struct get_primary<HeadTag, HeadCom, Components...>
  {
    using type = get_primary_t<Components...>;
  };

  template<typename HeadTag, typename HeadCom>
  struct get_primary<HeadTag, HeadCom>
  {
    using type = primary<void>;
  };

  template<typename HeadCom, typename... Components>
  struct get_primary<component_kinds::normal, HeadCom, Components...>
  {
    using type = primary<HeadCom>;
  };

  template<typename HeadCom>
  struct get_primary<component_kinds::normal, HeadCom>
  {
    using type = primary<HeadCom>;
  };

  // appliers
  template<typename Traits>
  struct applier_helper
  {
    using component = typename Traits::component;

    template<typename Visitor>
    static void dispatch(component_kinds::normal,
                         World& world,
                         ent_id eid,
                         Visitor&& visitor)
    {
      auto& com = world.template get_component<component>(eid);
      visitor(com);
    }

    template<typename Visitor>
    static void dispatch(component_kinds::ent,
                         World&,
                         ent_id eid,
                         Visitor&& visitor)
    {
      visitor(eid);
    }

    template<typename Visitor>
    static void dispatch(component_kinds::world,
                         World& world,
                         ent_id,
                         Visitor&& visitor)
    {
      visitor(world);
    }

    template<typename Visitor>
    static void dispatch(component_kinds::tagged,
                         World& world,
                         ent_id eid,
                         Visitor&& visitor)
    {
      if (world.template has_component<component>(eid)) {
        visitor(component{});
      }
    }
  };

  template<typename PrimaryComponent, typename... Components>
  struct applier;

  template<typename PrimaryComponent, typename HeadCom, typename... TailComs>
  struct applier<primary<PrimaryComponent>, HeadCom, TailComs...>
  {
    using next_applier = applier<primary<PrimaryComponent>, TailComs...>;

    template<typename Visitor, typename... Args>
    static void try_apply(World& world,
                          ent_id eid,
                          com_id primary_cid,
                          Visitor&& visitor,
                          Args&&... args)
    {
      using Traits = comp_kind<HeadCom>;
      applier_helper<Traits>::dispatch(
        typename Traits::category{}, world, eid, [&](auto&& new_arg) {
          next_applier::try_apply(world,
                                  eid,
                                  primary_cid,
                                  std::forward<Visitor>(visitor),
                                  std::forward<Args>(args)...,
                                  std::forward<decltype(new_arg)>(new_arg));
        });
    }
  };

  template<typename PrimaryComponent, typename... TailComs>
  struct applier<primary<PrimaryComponent>, PrimaryComponent, TailComs...>
  {
    using next_applier = applier<primary<PrimaryComponent>, TailComs...>;

    template<typename Visitor, typename... Args>
    static void try_apply(World& world,
                          ent_id eid,
                          com_id primary_cid,
                          Visitor&& visitor,
                          Args&&... args)
    {
      using Traits = comp_kind<PrimaryComponent>;
      auto& com =
        world.template get_component_by_id<typename Traits::component>(
          primary_cid);
      next_applier::try_apply(world,
                              eid,
                              primary_cid,
                              std::forward<Visitor>(visitor),
                              std::forward<Args>(args)...,
                              com);
    }
  };

  template<typename PrimaryComponent>
  struct applier<primary<PrimaryComponent>>
  {
    template<typename Visitor, typename... Args>
    static void try_apply(World&,
                          ent_id,
                          com_id,
                          Visitor&& visitor,
                          Args&&... args)
    {
      std::forward<Visitor>(visitor)(std::forward<Args>(args)...);
    }
  };

  // key

  template<typename PrimaryComponent, typename... Components>
  struct visitor_key;

  template<typename PrimaryComponent, typename HeadCom, typename... TailComs>
  struct visitor_key<primary<PrimaryComponent>, HeadCom, TailComs...>
  {
    using traits = comp_kind<HeadCom>;
    using next_key = visitor_key<primary<PrimaryComponent>, TailComs...>;

    static bool helper(World& world, ent_id eid, component_kinds::std)
    {
      return next_key::check(world, eid) &&
             world.template has_component<typename traits::component>(eid);
    }

    static bool helper(World& db, ent_id eid, component_kinds::meta)
    {
      return next_key::check(db, eid);
    }

    static bool check(World& world, ent_id eid)
    {
      return helper(world, eid, typename traits::category{});
    }
  };

  template<typename PrimaryComponent, typename... TailComs>
  struct visitor_key<primary<PrimaryComponent>, PrimaryComponent, TailComs...>
  {
    using next_key = visitor_key<primary<PrimaryComponent>, TailComs...>;

    static bool check(World& world, ent_id eid)
    {
      return next_key::check(world, eid);
    }
  };

  template<typename PrimaryComponent>
  struct visitor_key<primary<PrimaryComponent>>
  {
    static bool check(World&, ent_id) { return true; }
  };

  // visitor traits
  template<typename... Components>
  struct visitor_traits_impl
  {
    using ent_id = typename World::ent_id;
    using com_id = typename World::com_id;
    using primary_component = get_primary_t<Components...>;
    using key = visitor_key<primary_component, Components...>;

    template<typename Visitor>
    static void apply(World& world,
                      ent_id eid,
                      com_id primary_cid,
                      Visitor&& visitor)
    {
      applier<primary_component, Components...>::try_apply(
        world, eid, primary_cid, std::forward<Visitor>(visitor));
    }
  };

  // no clue
  template<typename Visitor>
  struct visitor_traits
    : visitor_traits<decltype(&std::decay_t<Visitor>::operator())>
  {};
  template<typename R, typename... Ts>
  struct visitor_traits<R (&)(Ts...)> : visitor_traits_impl<std::decay_t<Ts>...>
  {};

  template<typename Visitor, typename R, typename... Ts>
  struct visitor_traits<R (Visitor::*)(Ts...)>
    : visitor_traits_impl<std::decay_t<Ts>...>
  {};

  template<typename Visitor, typename R, typename... Ts>
  struct visitor_traits<R (Visitor::*)(Ts...) const>
    : visitor_traits_impl<std::decay_t<Ts>...>
  {};

  template<typename Visitor, typename R, typename... Ts>
  struct visitor_traits<R (Visitor::*)(Ts...)&>
    : visitor_traits_impl<std::decay_t<Ts>...>
  {};

  template<typename Visitor, typename R, typename... Ts>
  struct visitor_traits<R (Visitor::*)(Ts...) const&>
    : visitor_traits_impl<std::decay_t<Ts>...>
  {};

  template<typename Visitor, typename R, typename... Ts>
  struct visitor_traits<R (Visitor::*)(Ts...) &&>
    : visitor_traits_impl<std::decay_t<Ts>...>
  {};
};

class World
{
public:
  using ent_id = std::vector<entity>::size_type;
  using com_id = component_set::size_type;

  ent_id create_entity()
  {
    ent_id eid;

    if (m_free_entities.empty()) {
      eid = m_entities.size();
      m_entities.emplace_back();
    } else {
      eid = m_free_entities.back();
      m_free_entities.pop_back();
    }
    m_entities[eid].components.set(0);
    return eid;
  }

  void destroy_entity(ent_id eid)
  {
    // this could probably be optimized
    for (size_t i = 1; i < m_entities[eid].components.size(); ++i) {
      if (m_entities[eid].components[i]) {
        m_component_sets[i]->remove(eid);
      }
    }
    m_entities[eid].components.reset();
    m_free_entities.push_back(eid);
  }

  template<typename T>
  com_id create_component(ent_id eid, T&& com)
  {
    using com_type = std::decay_t<T>;
    auto guid = get_type_ct<com_type>();
    auto& ent_coms = m_entities[eid].components;
    auto& com_set = get_or_create_com_set<com_type>();

    com_id cid;

    if (guid < ent_coms.size() && ent_coms[guid]) {
      cid = com_set.get_comid(eid);
      com_set.get_com(cid) = std::forward<T>(com);
    } else {
      cid = com_set.assign(eid, std::forward<T>(com));
      ent_coms.set(guid);
    }
    return cid;
  }

  template<typename Com>
  void destroy_component(ent_id eid)
  {
    auto guid = get_type_ct<Com>();
    auto& com_set = *get_com_set<Com>();
    com_set.remove(eid);
    m_entities[eid].components.reset(guid);
  }

  template<typename Com>
  Com& get_component(ent_id eid)
  {
    auto& com_set = *get_com_set<Com>();
    auto cid = com_set.get_comid(eid);
    return com_set.get_com(cid);
  }

  template<typename Com>
  Com& get_component_by_id(com_id cid)
  {
    auto& com_set = *get_com_set<Com>();
    return com_set.get_com(cid);
  }

  template<typename Com>
  bool has_component(ent_id eid)
  {
    auto guid = get_type_ct<Com>();
    auto& ent_coms = m_entities[eid].components;
    return guid < ent_coms.size() && ent_coms[guid];
  }

  template<typename Visitor>
  void visit(Visitor&& visitor)
  {
    using w_traits = world_traits<World>;
    using traits = typename w_traits::visitor_traits<Visitor>;
    using primary_component = typename traits::primary_component;

    return visit_helper(std::forward<Visitor>(visitor), primary_component{});
  }

private:
  std::vector<entity> m_entities;
  std::vector<ent_id> m_free_entities;
  std::vector<std::unique_ptr<component_set>> m_component_sets;

  template<typename Com>
  component_set_impl<Com>* get_com_set()
  {
    auto guid = get_type_ct<Com>();
    if (guid >= m_component_sets.size()) {
      return nullptr;
    }
    auto& com_set = m_component_sets[guid];
    auto com_set_impl = static_cast<component_set_impl<Com>*>(com_set.get());
    return com_set_impl;
  }

  template<typename Com>
  component_set_impl<Com>& get_or_create_com_set()
  {
    auto guid = get_type_ct<Com>();
    if (m_component_sets.size() <= guid) {
      m_component_sets.resize(guid + 1);
    }
    auto& com_set = m_component_sets[guid];
    if (!com_set) {
      com_set = std::make_unique<component_set_impl<Com>>();
    }
    auto com_set_impl = static_cast<component_set_impl<Com>*>(com_set.get());
    return *com_set_impl;
  }

  template<typename Visitor, typename Component>
  void visit_helper(Visitor&& visitor, primary<Component>)
  {
    using w_traits = world_traits<World>;
    using traits = typename w_traits::visitor_traits<Visitor>;
    using key = typename traits::key;

    if (auto com_set_ptr = get_com_set<Component>()) {
      auto& com_set = *com_set_ptr;

      for (com_id cid = 0; cid < com_set.size(); ++cid) {
        auto eid = com_set.get_entid(cid);
        if (key::check(*this, eid)) {
          traits::apply(*this, eid, cid, visitor);
        }
      }
    }
  }

  template<typename Visitor, typename Component>
  void visit_helper(Visitor&& visitor, primary<void>)
  {
    using w_traits = world_traits<World>;
    using traits = typename w_traits::visitor_traits<Visitor>;
    using key = typename traits::key;

    for (com_id eid = 0; eid < m_entities.size(); ++eid) {
      if (m_entities[eid].components[eid] && key::check(*this, eid)) {
        traits::apply(*this, eid, {}, visitor);
      }
    }
  }
};

} // ecs
#endif /* ECS_HPP */
