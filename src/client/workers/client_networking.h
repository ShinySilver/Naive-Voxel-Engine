//
// Created by silver on 10/10/22.
//

#ifndef IVY_CLIENT_NETWORKING_H
#define IVY_CLIENT_NETWORKING_H

#include <functional>
#include "../utils/chunk_cache.h"

namespace client_networking {
    /**
     * Initialize the client networking workers & queues
     */
    void init();

    /**
     * Call the destructor of the given entity, and remove it from the loaded cells. The entity can be re-used or freed.
     * @param entity
     * @deprecated
     */
    void unload_cell(const Entity *entity);

    /**
     * Ask the server for some worldgen. When it's done, run the callback with it.
     * @param cell_coordinate
     * @param callback
     * @deprecated
     */
    void load_cell_async(const glm::vec3 cell_coordinate, std::function<void(Chunk *cell)> callback);
};


#endif //IVY_CLIENT_NETWORKING_H
