//
// Created by silver on 30/10/22.
//

#ifndef IVY_CHUNK_LOADING_H
#define IVY_CHUNK_LOADING_H


#include "../common/utils/safe_queue.h"
#include "../common/entities/entity.h"
#include "chunk_cache.h"

namespace chunk_loading {

    using chunk_cache::ChunkCacheEntry;

    /**
     * The different queues used by both the client & the workers defined here
     * TODO: Think about a way to share 'em with the client without exposing them.
     */
    extern SafeQueue<ChunkCacheEntry *> preloading_queue;
    extern SafeQueue<ChunkCacheEntry *> cascading_loading_queue;
    extern SafeQueue<Entity *> loading_queue;
    extern SafeQueue<Entity *> unloading_queue;

    /**
     * Initialize the chunk buffer, fill it, and save the camera position
     */
    void init();

    /**
     * When the camera position change, queue chunk preload/unload using the chunk buffer. Sometimes, we might want to
     * unload chunks that are still loading in order to load another chunk in their place. In this case, we do replace
     * them with a new chunk in the cache, but we also add them to a special vector to remember unloading 'em ASAP.
     * The next tick, the worker will remember to check these chunks, even if the camera did not move.
     *
     * TODO: The current chunk loading algorithm is worse than what's described above. Fix it?
     * TODO: It would be nice to have different loading / unloading range, to avoid chunk flickering when at a border.
     */
    void main_worker_tick();

    /**
     * Tasked with chunk preloading.
     */
    void worker_tick();
};


#endif //IVY_CHUNK_LOADING_H
