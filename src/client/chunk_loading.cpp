//
// Created by silver on 30/10/22.
//

#include "client_networking.h"
#include "chunk_loading.h"
#include "loguru.hpp"
#include "camera.h"
#include "client.h"
#include "chunk_cache.h"
#include "utils/meshing/greedy_mesher.h"

namespace chunk_loading {

    using chunk_cache::ChunkCacheEntry;

    SafeQueue<ChunkCacheEntry *> preloading_queue;
    SafeQueue<Entity *> loading_queue;
    SafeQueue<Entity *> unloading_queue;

    void init() {
        /**
         * Initializing the chunk map
         */
    }

    void main_worker_tick() {
        static ChunkPos last_pos = location_to_chunk_pos(camera::get_location());
        ChunkPos new_pos = location_to_chunk_pos(camera::get_location());

        // if (last_pos == new_pos) {
        //     std::this_thread::sleep_for(std::chrono::milliseconds(50));
        //     return;
        // }

        ChunkCacheEntry *entry;
        for (int dx = -VIEW_DISTANCE; dx <= VIEW_DISTANCE; ++dx) {
            for (int dy = -VIEW_DISTANCE; dy <= VIEW_DISTANCE; ++dy) {
                for (int dz = -VIEW_DISTANCE; dz <= VIEW_DISTANCE; ++dz) {
                    ChunkPos p = {new_pos.x + dx, new_pos.y + dy, new_pos.z + dz};
                    entry = chunk_cache::get_cache_entry(p);

                    // Skipping if the chunk is correctly placed
                    if (entry->position == p || entry->entity != nullptr && !entry->entity->is_loaded()
                        || entry->is_awaiting_mesh || entry->is_awaiting_voxels)
                        continue;

                    /*
                                         if (entry->entity != nullptr) {
                        if (entry->is_awaiting_mesh) {
                            queue.lock();
                            if (queue.has(entry)) {
                                queue.remove(entry);
                            }else{
                                queue.unlock();
                                continue;
                            }
                            queue.unlock();
                        } else if (entry->is_awaiting_voxels) {

                        }
                    }
                     * */

                    // TODO what if we are meshing a neighbour and the chunk disappear? NPE?
                    // When we start meshing, for each nearby chunk we try to lock it. If we fail, we silently stop the
                    // meshing. In the main func, we try to lock every chunk in the unload func. if we fail, we add the
                    // chunk back to the unloading queue. In order to avoid deadlocks, we now limit the amount of chunk
                    // load/unload per render tick, and at most we unload N chunks, N being the queue size at the start
                    // of the unloading loop.

                    // Now that we know that it is misplaced, if applicable enqueue the entity for unloading.
                    if (entry->entity != nullptr) {
                        unloading_queue.enqueue((Entity *) entry->entity);
                        entry->entity = nullptr;
                    }

                    // Now that it's done, let's update the entry with a new position, so we don't regen the chunk again
                    entry->position = p;

                    // At last, we add it to the world gen queue without creating an entity yet - it might be air!
                    entry->is_awaiting_voxels = true; // Makes the difference between a chunk with air & a pending one
                    client_networking::fill_chunk_cache_entry(entry); // TODO implement it
                }
            }
        }

        last_pos = new_pos;
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    void worker_tick() {
        ChunkCacheEntry *entry = preloading_queue.dequeue();
        if (entry != nullptr && !entry->is_air) { // mallocé mais pas créé!
            DLOG_S(4) << "Preloading chunk at chunk pos "
                      << entry->position.x << ";"
                      << entry->position.y << ";"
                      << entry->position.z << "";
            Mesh *mesh = GreedyMesher::mesh(*entry->chunk_data);
            entry->entity = new EntityChunk(mesh, Location(chunk_pos_to_world_pos(entry->position)));
            entry->entity->preload();
            entry->is_awaiting_mesh = false;
            loading_queue.enqueue(entry->entity);
        }
    }
}