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
    }

    void uncull_neighbours(ChunkCacheEntry *pEntry);

    void main_worker_tick() {
        static ChunkPos last_pos = ChunkPos(FLT_MAX, FLT_MAX, FLT_MAX);
        ChunkPos new_pos = location_to_chunk_pos(camera::get_location());

        if (last_pos == new_pos) {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            return;
        }

        ChunkCacheEntry *entry;
        for (int dx = -VIEW_DISTANCE; dx <= VIEW_DISTANCE; ++dx) {
            for (int dy = -VIEW_DISTANCE; dy <= VIEW_DISTANCE; ++dy) {
                for (int dz = -VIEW_DISTANCE; dz <= VIEW_DISTANCE; ++dz) {
                    ChunkPos p = {new_pos.x + dx, new_pos.y + dy, new_pos.z + dz};
                    entry = chunk_cache::get_cache_entry(p);

                    // Unload if the chunk is past the render distance
                    // TBD

                    // Skipping if the chunk is correctly placed or if the chunk is marked as culled
                    if (entry->position == p || entry->entity != nullptr && !entry->entity->is_loaded() ||
                        entry->is_awaiting_mesh || entry->is_awaiting_voxels) {
                        continue;
                    }

                    if (entry->entity != nullptr) {
                        entry->should_cull = true;
                        unloading_queue.enqueue(entry->entity);
                        entry->entity = nullptr;
                    }

                    if (!(!dx && !dy && !dz) && entry->should_cull) {
                        continue;
                    }

                    // Now that it's done, let's update the entry with a new position, so we don't regen the chunk again
                    entry->position = p;

                    // At last, we add it to the world gen queue without creating an entity yet - it might be air!
                    entry->is_awaiting_voxels = true; // Some status update
                    client_networking::load_cell_async(entry->position, [entry](Chunk *new_chunk) {
                        if (new_chunk != nullptr) {
                            entry->is_air = false;

#if KEEP_CHUNKS_DATA_IN_MEMORY == false
                            if (entry->chunk_data != nullptr) {
                                delete (entry->chunk_data);
                                entry->chunk_data = nullptr;
                            }
#endif

                            entry->chunk_data = new_chunk;
                            entry->is_awaiting_mesh = true;
                            entry->is_awaiting_voxels = false;
                            chunk_loading::preloading_queue.enqueue(entry);
                        } else {
                            entry->is_air = true;
                            entry->is_awaiting_voxels = false;
                            chunk_loading::uncull_neighbours(entry);
                        }
                    });
                }
            }
        }

        last_pos = new_pos;
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    void uncull_neighbours(ChunkCacheEntry *entry) {
        return;
    }

    void worker_tick() {
        ChunkCacheEntry *entry = preloading_queue.dequeue();
        if (entry != nullptr && !entry->is_air) { // mallocé mais pas créé!
            DLOG_S(4) << "Preloading chunk at chunk pos "
                      << entry->position.x << ";"
                      << entry->position.y << ";"
                      << entry->position.z << "";
            Mesh *mesh = GreedyMesher::mesh(*entry->chunk_data, nullptr);
#if KEEP_CHUNKS_DATA_IN_MEMORY == false
            delete (entry->chunk_data);
            entry->chunk_data = nullptr;
#endif
            if (entry->entity != nullptr) {
                unloading_queue.enqueue((Entity *) entry->entity);
            }
            entry->entity = new EntityChunk(mesh, Location(chunk_pos_to_world_pos(entry->position)));
            entry->entity->preload();
            entry->is_awaiting_mesh = false;
            loading_queue.enqueue(entry->entity);
        }
    }
}