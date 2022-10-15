//
// Created by silver on 10/10/22.
//

#ifndef IVY_CLIENT_NETWORKING_H
#define IVY_CLIENT_NETWORKING_H

#include "../common/world/entities/entity.h"

namespace client_networking {
    /**
     * Initialize the client networking workers & queues
     */
    void init();

    /**
     * Call the destructor of the given entity, and remove it from the loaded cells. The entity can be re-used or freed.
     * @param entity
     */
    void unload_cell(const Entity *entity);

    /**
     * Use placement new to load a chunk in the given allocation, then run the callback with it.
     * @param cell_coordinate
     * @param entity
     * @param callback
     */
    void load_cell_async(const glm::vec3 &cell_coordinate, Entity *entity, std::function<void(Entity *cell)> callback);
};


#endif //IVY_CLIENT_NETWORKING_H
