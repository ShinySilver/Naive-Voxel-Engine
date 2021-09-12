#ifndef IVY_WORLD_H
#define IVY_WORLD_H

#include "world/entities/entity.h"

#include <vector>

namespace world {

void init();

std::vector<Entity*>& get_entities();

}

#endif //IVY_WORLD_H

