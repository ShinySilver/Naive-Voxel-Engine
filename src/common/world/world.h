#ifndef IVY_WORLD_H
#define IVY_WORLD_H

#include "entities/entity.h"
#include <bits/stdc++.h>

#include <vector>

namespace world {

    void init();

    void get_loaded_chunks();
    void tick();

    std::vector<Entity *> get_cell(const glm::vec3 &cell_coordinate);

    void get_cell_async(glm::vec3 &cell_coordinate, 
			std::function<void(const std::vector<Entity *> &cell)> callback);

    std::vector<Entity *> &get_entities();

}

#endif //IVY_WORLD_H

