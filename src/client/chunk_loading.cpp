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
#include "../common/utils/direction.h"

namespace chunk_loading {

    using chunk_cache::ChunkCacheEntry;

    SafeQueue<ChunkCacheEntry *> preloading_queue;
    SafeQueue<Entity *> loading_queue;
    SafeQueue<Entity *> unloading_queue;
    SafeQueue<ChunkPos *> cascading_loading_queue;

    void init() {}

    void main_worker_tick() {
        static ChunkPos last_pos = ChunkPos(FLT_MAX, FLT_MAX, FLT_MAX);
        ChunkPos new_pos = location_to_chunk_pos(camera::get_location());

        /*if (last_pos == new_pos) {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            return;
        }*/

        ChunkCacheEntry *entry;
        ChunkPos p;
        for (int dx = -VIEW_DISTANCE; dx <= VIEW_DISTANCE; ++dx) {
            for (int dy = -VIEW_DISTANCE; dy <= VIEW_DISTANCE; ++dy) {
                for (int dz = -VIEW_DISTANCE; dz <= VIEW_DISTANCE; ++dz) {
                    p = {new_pos.x + dx, new_pos.y + dy, new_pos.z + dz};
                    entry = chunk_cache::get_cache_entry(p);

                    // Skipping if the chunk is correctly placed or if the entry is busy generating
                    if (entry->in_processing || entry->position == p) {
                        continue;
                    }

                    // Since we now know the chunk that's in the entry is wrongly placed, we unload it
                    if (entry->entity != nullptr) {
                        unloading_queue.enqueue(entry->entity);
                        entry->entity = nullptr;
                    }

                    // Not loading chunk that are marked as culled (aside from the player's chunk)
                    if (entry->last_valid_pos != p && !(!dx && !dy && !dz)) {
                        continue;
                    }

                    // Now that it's done, let's update the entry with a new position, so we don't regen the chunk again
                    entry->position = p;

                    // At last, we add it to the world gen queue without creating an entity yet - it might be air!
                    entry->in_processing = true;
                    client_networking::load_cell_async(entry->position, [entry](Chunk *new_chunk) {
                        if (new_chunk != nullptr) {
                            entry->is_air = false;

                            entry->chunk_data = new_chunk;
                            entry->is_awaiting_mesh = true;
                            entry->is_awaiting_voxels = false;
                            cascading_loading_queue.enqueue(new ChunkPos(entry->position));
                            preloading_queue.enqueue(entry);
                        } else {
                            entry->is_air = true;
                            entry->is_awaiting_voxels = false;
                            cascading_loading_queue.enqueue(new ChunkPos(entry->position));
                            entry->in_processing = false;
                        }
                    });
                }
            }
        }

        ChunkPos *pos;
        int queue_size = cascading_loading_queue.size();
        for (int i = 0; i < queue_size; i++) {
            pos = cascading_loading_queue.dequeue();

            // First of all, we don't try to propagate worldgen out of the hard view distance
            glm::vec3 dist_to_player = *pos - new_pos;
            if (std::abs(dist_to_player.x) > VIEW_DISTANCE ||
                std::abs(dist_to_player.y) > VIEW_DISTANCE ||
                std::abs(dist_to_player.z) > VIEW_DISTANCE) {
                continue;
            }

            // Then we mark all the neighbours
            for (auto direction: Directions::All) {
                p = *pos + direction;

                chunk_cache::get_cache_entry(p)->last_valid_pos = p;
            }
        }

        last_pos = new_pos;
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    void worker_tick() {
        ChunkCacheEntry *entry = preloading_queue.dequeue();
        if (entry != nullptr && !entry->is_air) {
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
            entry->in_processing = false;
        }
    }
}