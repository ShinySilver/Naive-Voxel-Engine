//
// Created by silver on 10/10/22.
//

#include <functional>
#include "client_networking.h"
#include "../server/world.h"
#include "../server/server_networking.h"
#include "chunk_loading.h"

namespace client_networking {
    namespace {

    }

    void init() {

    }

    void unload_cell(const Entity *entity) {
        // This code will send a request through a socket & return. But for now, it just directly run it.
    }

    /**
     * @deprecated
     */
    void load_cell_async(const glm::vec3 cell_coordinate, std::function<void(Chunk *cell)> callback) {
        // This code will send a request through a socket, register the callback & return. The callback will be run
        // when the chunk is generated. But for now, it will directly add the chunk request to the server worker task
        // list, and the callback will be called by the server. Thus, the callback must be light!
        server_networking::queue_chunk_generation_request(cell_coordinate, std::move(callback));
    }
}