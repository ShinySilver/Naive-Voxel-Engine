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
    SafeQueue<ChunkCacheEntry *> cascading_loading_queue;

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

                    if (entry->position != p) {
                        entry->should_cull = true;
                    }

                    // Skipping if the chunk is correctly placed or if the entry is marked as already generating
                    if (entry->position == p && !entry->in_processing && entry->entity != nullptr ||
                        entry->in_processing) {
                        continue;
                    }

                    // Ignoring chunk that are marked as culled (aside from the player's chunk)
                    if (!(!dx && !dy && !dz) && entry->should_cull) {
                        continue;
                    }

                    // Since we now know we WILL generate a chunk and put it in the entry, we reset the entry
                    if (entry->entity != nullptr) {
                        unloading_queue.enqueue(entry->entity);
                        entry->entity = nullptr;
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
                            cascading_loading_queue.enqueue(entry);
                            preloading_queue.enqueue(entry);
                        } else {
                            entry->is_air = true;
                            entry->is_awaiting_voxels = false;
                            cascading_loading_queue.enqueue(entry);
                        }
                    });
                }
            }
        }

        ChunkCacheEntry *neighbour;
        int queue_size = cascading_loading_queue.size();
        for (int i = 0; i < queue_size; i++) {
            entry = cascading_loading_queue.dequeue();
            for (auto direction: Directions::All) {
                p = entry->position + direction;
                neighbour = chunk_cache::get_cache_entry(p);

                // If the chunk we want to mark as not to cull has invalid chunk pos and is not loading,
                // we can fix chunk pos and mark it IF WE DON'T HAVE A FULL FACE LOOKING TOWARDS IT
                if (neighbour->position != p) {
                    if (!neighbour->in_processing) {
                        if (entry->chunk_data == nullptr || entry->chunk_data->has_full_face(direction)) continue;

#if KEEP_CHUNKS_DATA_IN_MEMORY == false
                        if (neighbour->chunk_data != nullptr) {
                            delete (entry->chunk_data);
                            neighbour->chunk_data = nullptr;
                        }
#endif

                        if (neighbour->entity != nullptr) {
                            unloading_queue.enqueue(entry->entity);
                            neighbour->entity = nullptr;
                        }

                        neighbour->position = p;
                        neighbour->should_cull = false;
                    } else {
                        // otherwise, we can only wait for it to load before fixing it. Annoying.
                        // TODO: make TWO queues, so that we don't have this check to do for most chunks.
                        glm::vec3 dist_to_player = neighbour->position - new_pos;
                        if (dist_to_player.x <= VIEW_DISTANCE + 1 || dist_to_player.y <= VIEW_DISTANCE + 1 ||
                            dist_to_player.z <= VIEW_DISTANCE + 1) {
                            cascading_loading_queue.enqueue(entry);
                        }
                    }
                }
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
            entry->in_processing = false;
            loading_queue.enqueue(entry->entity);
        }
    }
}