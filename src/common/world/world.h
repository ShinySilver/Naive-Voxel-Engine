#ifndef IVY_WORLD_H
#define IVY_WORLD_H

#include "world/entities/entity.h"

#include <vector>

namespace world {

void init();

world_provider &get_world_provider();
std::vector<Entity*>& get_entities();

}

#endif //IVY_WORLD_H

