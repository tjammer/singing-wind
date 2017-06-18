//
// Created by jammer on 18/05/17.
//

#ifndef SINGING_WIND_SCENEIO_H
#define SINGING_WIND_SCENEIO_H

#include <Scene.pb.h>


scene::Entity * get_pb_entity(GameWorld &game_world, unsigned int entity);

bool save_entity_standalone(GameWorld &game_world, unsigned int entity);

void entity_to_world(const scene::Entity& entity, GameWorld &game_world, unsigned int);

bool load_entity_from_filename(const std::__cxx11::string &name, GameWorld &game_world, unsigned int entity);

void delete_entity_from_scene(GameWorld &game_world, unsigned int entity);

// loads entity by name and changes position
void scene_entity_to_world(const scene::Entity& pb_entity, GameWorld &game_world, unsigned int);


#endif //SINGING_WIND_SCENEIO_H
