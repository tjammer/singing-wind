//
// Created by jammer on 07/04/17.
//

#ifndef SINGING_WIND_GAMEWORLD_H
#define SINGING_WIND_GAMEWORLD_H

#include <string>
#include <vector>
#include <memory>
#include "WindDefs.h"
#include "ColGrid.h"
#include "NavMesh.h"
#include "CPruneSweep.h"

struct GLFWwindow;
struct NavMesh;
class Island;

// physics constants
const float c_drag = 0.0026;
const float c_max_floor_angle = 0.7;
const float c_gravity = 980;
const float c_jump_tolerance = 0.1f;

struct EntityCreator
{
  std::function<void(GameWorld&, unsigned int new_ent, unsigned int parent)>
    func;
  unsigned int parent;
};

// implements the ecs
class GameWorld
{
public:
  GameWorld();
  ~GameWorld();

  // maybe a timer?
  void step_fixed(float dt);
  void pre_draw(float dt);
  void draw();

  std::vector<bset>& entities();
  std::vector<bset> const& entities() const;

  void reset_entities();
  void reset_islands();

  // delete entities
  void queue_delete(unsigned int entity);

  // communication with editor
  void update_world();

  unsigned int create_entity();
  unsigned int load_entity(const std::string& name);
  unsigned int create_root();
  void delete_entity_raw(unsigned int entity);

  // getters
  template<typename C>
  C& get(unsigned int entity);

  HashGrid<StaticTriangle>& grid();
  HashGrid<StaticTriangle> const& grid() const;
  PruneSweeper& prune_sweep();
  NavMesh& navmesh();
  NavMesh const& navmesh() const;
  std::vector<Island>& islands();
  std::vector<Island> const& islands() const;

private:
  class impl;
  std::unique_ptr<impl> pimpl;

  void find_entities_fixed();
  void find_entities_draw();
};

namespace for_gameworld {
inline bool
has_component(bset entity, bset component_mask)
{
  return (entity & component_mask) == component_mask;
}
}

#endif // SINGING_WIND_GAMEWORLD_H
