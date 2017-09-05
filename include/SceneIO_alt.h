#ifndef SCENEIO_ALT_H
#define SCENEIO_ALT_H
#include "entity_generated.h"

class GameWorld;

bool save_entity_standalone_fbs(GameWorld &game_world, unsigned int entity);

void entity_to_world_fbs(const EntityFBS::EntityT&, GameWorld &world, unsigned int entity);

bool load_entity_from_filename_fbs(const std::string &name, GameWorld &world, unsigned int entity);

void save_scene_fbs(const std::string &, GameWorld &, float zoom);
bool load_scene_fbs(const std::string &, GameWorld &, float &zoom);

#endif /* SCENEIO_ALT_H */
