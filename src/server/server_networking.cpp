//
// Created by silver on 11/10/22.
//

#include <iostream>
#include "server_networking.h"
#include "../common/utils/safe_queue.h"
#include "../common/utils/worker.h"
#include "../common/entities/entity.h"
#include "glm/glm/vec3.hpp"
#include "world.h"
#include "generator.h"
#include "loguru.hpp"

namespace server_networking {
    namespace {
        struct ChunkGenRequest {
            const glm::vec3 cell_coordinate{};
            std::function<void(Chunk *chunk)> callback{};

            ChunkGenRequest(glm::vec3 cell_coordinate,
                            std::function<void(Chunk *chunk)> callback) : cell_coordinate(cell_coordinate),
                                                                          callback(callback) {}
			~ChunkGenRequest() = default;
        };

        std::vector<Worker *> workers;
        SafeQueue<std::shared_ptr<ChunkGenRequest>> generation_queue;

        void worker_tick() {
            auto e = generation_queue.dequeue();
            if (e) {
                Chunk *c = generator::generate(e->cell_coordinate); // TODO: refactor chunk creation
                e->callback(c);
            }
        }
    }

    void init() {
        LOG_S(1) << "Server starting its worker threads...." << std::endl;
        workers = std::vector<Worker *>();
        for (int i = 0; i < GENERATION_WORKER_COUNT; ++i) {
            workers.emplace_back(new Worker("world_generation_server_worker", worker_tick));
        }
    }

    void stop() {
        LOG_S(1) << "Unlocking worldgen worker queue. Server workers stopping...\n";
        for (int i = 0; i < GENERATION_WORKER_COUNT; ++i) {
            workers[i]->stop();
        }
        generation_queue.unlock_all();
    }

    void join() {
        LOG_S(1) << "Waiting for worldgen workers to stop...\n";
        for (int i = 0; i < GENERATION_WORKER_COUNT; ++i) {
            workers[i]->join();
            delete workers[i];
        }
    }

    void queue_chunk_generation_request(const glm::vec3 cell_coordinate,
                                        std::function<void(Chunk *chunk)> callback) {
        generation_queue.enqueue(std::make_shared<ChunkGenRequest>(cell_coordinate, callback)); // TODO: check C++11 for move
    }


}
