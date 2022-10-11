//
// Created by silver on 11/10/22.
//

#include <iostream>
#include "server_networking.h"
#include "../common/utils/safe_queue.h"
#include "../common/utils/worker.h"
#include "../common/world/world.h"

namespace server_networking {
    namespace {
        struct ChunkGenRequest {
            const glm::vec3 &cell_coordinate;
            Entity *entity;
            ChunkGenRequest(const glm::vec3 &cell_coordinate, Entity *entity) : entity(entity),
                                                                                cell_coordinate(cell_coordinate) {}
        };

        std::vector<Worker *> workers;
        SafeQueue<ChunkGenRequest *> generation_queue;

        void worker_tick() {
            ChunkGenRequest *e = generation_queue.dequeue();
            if (e) {
                world::load_cell(e->cell_coordinate, e->entity);
            }
        }
    }

    void init() {
        std::cout << "Server starting its worker threads...." << std::endl;
        workers = std::vector<Worker *>();
        for (int i = 0; i < 3; ++i) {
            workers.emplace_back(new Worker("world_generation_server_worker", worker_tick));
        }
    }

    void stop() {
        std::cout << "Unlocking world generation queue. Server workers stopping...\n";
        generation_queue.unlock_all();
        for (int i = 0; i < 3; ++i) {
            workers[i]->stop();
        }

    }

    void join() {
        std::cout << "Waiting for server workers to stop...\n";
        for (int i = 0; i < 3; ++i) {
            workers[i]->join();
            delete workers[i];
        }
    }

    void queue_chunk_request() {

    }

    void queue_chunk_generation_request(const glm::vec3 &cell_coordinate, Entity *entity) {
        generation_queue.enqueue(new ChunkGenRequest(cell_coordinate, entity));
    }

}