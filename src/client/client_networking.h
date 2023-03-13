//
// Created by silver on 10/10/22.
//

#ifndef IVY_CLIENT_NETWORKING_H
#define IVY_CLIENT_NETWORKING_H

#include <functional>
#include "chunk_cache.h"

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
     * Use placement new to load a chunk in the given allocation, then run the callback with it.
     * @param cell_coordinate
     * @param entity
     * @param callback
     * @deprecated
     */
    void load_cell_async(const glm::vec3 cell_coordinate, Entity *entity, std::function<void(Entity *cell)> callback);

    /**
     * Ask the server for some worldgen. when it's done, add it to the cache, update it & its neighbour, and eventually
     * add one or multiple of 'em to the meshing queue.
     * @param pEntry
     */
    void fill_chunk_cache_entry(chunk_cache::ChunkCacheEntry *cache_entry);
};


#endif //IVY_CLIENT_NETWORKING_H
