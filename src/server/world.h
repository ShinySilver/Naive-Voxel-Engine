//
// Created by silverly on 20/05/2021.
//

#ifndef IVY_WORLD_H
#define IVY_WORLD_H

#include <bits/stdc++.h>
#include <vector>
#include "../common/entities/entity.h"

namespace world {

    /**
     * Initialize the world. Called during server initialization.
     */
    void init();

    /**
     * Tick the world. Called during server tick.
     */
    void tick();

    /**
     * @return the list of all loaded cells
     */
    std::vector<Entity *> &get_loaded_cells();

    /**
     * Call the destructor of the given entity, and remove it from the loaded cells. The entity can be re-used or freed.
     * @param entity
     */
    void unload_cell(const Entity *entity);

    /**
     * Use placement new to load a chunk in the given allocation.
     * @param cell_coordinate
     * @param entity
     */
    void load_cell(const glm::vec3 &cell_coordinate, Entity *entity);

    /**
     * Use placement new to load a chunk in the given allocation, then run the callback with it.
     * @param cell_coordinate
     * @param entity
     * @param callback
     */
    void load_cell_with_callback(const glm::vec3 &cell_coordinate, Entity *entity,
                                 std::function<void(Entity *cell)> callback);
}

#endif //IVY_WORLD_H

