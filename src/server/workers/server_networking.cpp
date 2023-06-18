//
// Created by silver on 11/10/22.
//

#include <iostream>
#include "server_networking.h"
#include "../../common/utils/safe_queue.h"
#include "../../common/utils/worker.h"
#include "glm/glm/vec3.hpp"
#include "loguru.hpp"
#include "../../common/world/chunk.h"

namespace server_networking {
    namespace {
        struct ChunkGenRequest {
            const glm::vec3 cell_coordinate{};
            std::function<void(Voxel * )> callback{};

            ChunkGenRequest(glm::vec3 cell_coordinate,
                            std::function<void(Voxel * )> callback) : cell_coordinate(cell_coordinate),
                                                                      callback(callback) {}
        };

        std::vector<Worker *> workers;
        SafeQueue<ChunkGenRequest *> generation_queue;

        void worker_tick() {
            ChunkGenRequest *e = generation_queue.dequeue();
            if (e) {
                clock_t t0 = clock();
                e->callback(nullptr);
                delete (e);
            }
        }
    }

    void init() {
        LOG_S(1) << "Server starting its worker threads....";
        workers = std::vector<Worker *>();
        for (int i = 0; i < GENERATION_WORKER_COUNT; ++i) {
            workers.emplace_back(new Worker("world_generation_server_worker", worker_tick));
        }
    }

    void stop() {
        LOG_S(1) << "Unlocking worldgen worker queue. Server workers stopping...";
        for (int i = 0; i < GENERATION_WORKER_COUNT; ++i) {
            workers[i]->stop();
        }
        generation_queue.unlock_all();
    }

    void join() {
        LOG_S(1) << "Waiting for worldgen workers to stop...";
        for (int i = 0; i < GENERATION_WORKER_COUNT; ++i) {
            workers[i]->join();
            delete workers[i];
        }
    }

    void queue_chunk_generation_request(const glm::vec3 cell_coordinate,
                                        std::function<void(Voxel *)> callback) {
        generation_queue.enqueue(new ChunkGenRequest(cell_coordinate, callback));
    }

}