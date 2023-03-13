//
// Created by silver on 30/10/22.
//

#include "client_networking.h"
#include "chunk_loading.h"
#include "loguru.hpp"
#include "camera.h"
#include "client.h"
#include "chunk_cache.h"

namespace chunk_loading {

    using chunk_cache::ChunkCacheEntry;

    SafeQueue<Entity *> preloading_queue;
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

        if (last_pos == new_pos) {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            return;
        }

        glm::vec3 delta = new_pos - last_pos;
        glm::vec3 delta_sign = glm::sign(delta);

        ChunkCacheEntry entry;
        for (int dx = 0; dx < CACHE_WIDTH; ++dx) {
            for (int dy = 0; dy < CACHE_WIDTH; ++dy) {
                for (int dz = 0; dz < CACHE_WIDTH; ++dz) {
                    ChunkPos p = {new_pos.x + dx, new_pos.y + dy, new_pos.z + dz};
                    entry = chunk_cache::get_cache_entry(p);

                    // Checking whether the chunk is misplaced.
                    if (entry.position == p || (entry.entity && !((Entity *) entry.entity)->is_loaded())) continue;

                    // TODO what if we are meshing a neighbour and the chunk disappear? NPE?
                    // When we start meshing, for each nearby chunk we try to lock it. If we fail, we silently stop the
                    // meshing. In the main func, we try to lock every chunk in the unload func. if we fail, we add the
                    // chunk back to the unloading queue. In order to avoid deadlocks, we now limit the amount of chunk
                    // load/unload per render tick, and at most we unload N chunks, N being the queue size at the start
                    // of the unloading loop.

                    // Now that we know that it is misplaced, if applicable enqueue the entity for unloading.
                    if (entry.entity != nullptr) unloading_queue.enqueue((Entity *) entry.entity);

                    // Now that it's done, let's update the entry with a new position, so we don't regen the chunk again
                    entry.position = p;

                    // At last, we add it to the world gen queue without creating an entity yet - it might be air!
                    entry.is_awaiting_voxels = true; // Makes the difference between a chunk with air & a pending one
                    client_networking::fill_chunk_cache_entry(&entry); // TODO implement it
                }
            }
        }

        last_pos = new_pos;
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    void worker_tick() {
        Entity *e = preloading_queue.dequeue();
        if (e) {
            ChunkPos chunk_pos = location_to_chunk_pos(e->getLocation());
            DLOG_S(4) << "Preloading chunk at chunk pos "
                      << chunk_pos.x << ";"
                      << chunk_pos.y << ";"
                      << chunk_pos.z << " and pos "
                      << e->getLocation().position.x << ";"
                      << e->getLocation().position.y << ";"
                      << e->getLocation().position.z << "";

            e->preload();
            loading_queue.enqueue(e);
        }
    }
}