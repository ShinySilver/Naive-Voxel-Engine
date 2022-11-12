//
// Created by silver on 30/10/22.
//

#include "client_networking.h"
#include "chunk_loading.h"
#include "client.h"
#include "camera.h"
#include "../common/utils/safe_queue.h"
#include "../common/world/entities/entity_chunk.h"
#include "../common/world/grid.h"
#include "loguru.hpp"

namespace chunk_loading {
    namespace {
        Entity *chunk_map[VIEW_DISTANCE * 2 + 1][VIEW_DISTANCE * 2 + 1][VIEW_DISTANCE * 2 + 1];
    }

    SafeQueue<Entity *> preloading_queue;
    SafeQueue<Entity *> loading_queue;
    SafeQueue<Entity *> unloading_queue;

    void init() {
        /**
         * Initializing chunk map
         */
        glm::vec3 player_pos = grid::pos_to_chunk(camera::get_location());
        Entity *e;
        for (int dx = -VIEW_DISTANCE; dx <= VIEW_DISTANCE; dx++) {
            for (int dy = -VIEW_DISTANCE; dy <= VIEW_DISTANCE; dy++) {
                for (int dz = -VIEW_DISTANCE; dz <= VIEW_DISTANCE; dz++) {
                    e = (Entity *) malloc(sizeof(EntityChunk));
                    client_networking::load_cell_async(player_pos + glm::vec3(dx, dy, dz), e,
                                                       [player_pos, dx, dy, dz](Entity *new_chunk) {
                                                           chunk_map[((INT_MAX / 2 + (int) player_pos.x + dx)) %
                                                                     (VIEW_DISTANCE * 2 + 1)][
                                                                   ((INT_MAX / 2 + (int) player_pos.y + dy)) %
                                                                   (VIEW_DISTANCE * 2 + 1)]
                                                           [((INT_MAX / 2 + (int) player_pos.z + dz)) %
                                                            (VIEW_DISTANCE * 2 + 1)] = new_chunk;
                                                           preloading_queue.enqueue(new_chunk);
                                                       });
                }
            }
        }
    }

    void main_worker_tick() {
        glm::vec3 player_pos = grid::pos_to_chunk(camera::get_location());
        Entity *e;
        for (int dx = -VIEW_DISTANCE; dx <= VIEW_DISTANCE; dx++) {
            for (int dy = -VIEW_DISTANCE; dy <= VIEW_DISTANCE; dy++) {
                for (int dz = -VIEW_DISTANCE; dz <= VIEW_DISTANCE; dz++) {
                    // We get the chunk in the cache corresponding to the given chunk pos
                    e = chunk_map[((INT_MAX / 2 + (int) player_pos.x + dx)) % (VIEW_DISTANCE * 2 + 1)][
                            ((INT_MAX / 2 + (int) player_pos.y + dy)) % (VIEW_DISTANCE * 2 + 1)]
                    [((INT_MAX / 2 + (int) player_pos.z + dz)) % (VIEW_DISTANCE * 2 + 1)];

                    // Skipping loading chunks - we are waiting for them to load before unloading 'em
                    if (e == 0 || !e->is_loaded())
                        continue;


                    // If the chunk is out of view distance, mark it for unload & replace it with a new one
                    if (grid::pos_to_chunk(e->getLocation()) != player_pos + glm::vec3(dx, dy, dz)) {
                        unloading_queue.enqueue(e);
                        chunk_map[((INT_MAX / 2 + (int) player_pos.x + dx)) % (VIEW_DISTANCE * 2 + 1)][
                                ((INT_MAX / 2 + (int) player_pos.y + dy)) % (VIEW_DISTANCE * 2 + 1)]
                        [((INT_MAX / 2 + (int) player_pos.z + dz)) % (VIEW_DISTANCE * 2 + 1)] = 0;
                        Entity *new_chunk = (Entity *) malloc(sizeof(EntityChunk));
                        client_networking::load_cell_async(player_pos + glm::vec3(dx, dy, dz), new_chunk,
                                                           [player_pos, dx, dy, dz](Entity *new_chunk) {
                                                               chunk_map[((INT_MAX / 2 + (int) player_pos.x + dx)) %
                                                                         (VIEW_DISTANCE * 2 + 1)][
                                                                       ((INT_MAX / 2 + (int) player_pos.y + dy)) %
                                                                       (VIEW_DISTANCE * 2 + 1)]
                                                               [((INT_MAX / 2 + (int) player_pos.z + dz)) %
                                                                (VIEW_DISTANCE * 2 + 1)] = new_chunk;
                                                               preloading_queue.enqueue(new_chunk);
                                                           });
                    }
                }
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    void worker_tick() {
        Entity *e = preloading_queue.dequeue();
        if (e) {
            glm::vec3 chunk_pos = grid::pos_to_chunk(e->getLocation());
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