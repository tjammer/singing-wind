#ifndef SCENEIO_ALT_H
#define SCENEIO_ALT_H
#include "entity_generated.h"

class GameWorld;

bool
save_entity_standalone(GameWorld& game_world, unsigned int entity);

void
entity_to_world(const EntityFBS::EntityT&,
                GameWorld& world,
                unsigned int entity);

bool
load_entity_from_filename(const std::string& name,
                          GameWorld& world,
                          unsigned int entity);

void
save_scene_to_fb(const std::string&, GameWorld&, float zoom);
bool
load_scene_from_fb(const std::string&, GameWorld&, float& zoom);

#endif /* SCENEIO_ALT_H */
