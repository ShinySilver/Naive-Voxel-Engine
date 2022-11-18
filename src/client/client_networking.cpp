//
// Created by silver on 10/10/22.
//

#include <functional>
#include "client_networking.h"
#include "../common/world/world.h"
#include "../server/server_networking.h"

namespace client_networking {
    void init(){

    }

    void unload_cell(const Entity *entity) {
        // This code will send a request through a socket & return. But for now, it just directly run it.
    }

    void
    load_cell_async(const glm::vec3 cell_coordinate, Entity *entity, std::function<void(Entity *cell)> callback) {
        // This code will send a request through a socket, register the callback & return. The callback will be run
        // when the chunk is generated. But for now, it will directly add the chunk request to the server worker task
        // list.
        //world::load_cell(cell_coordinate, entity);
        //callback(entity);

        server_networking::queue_chunk_generation_request(cell_coordinate, entity, callback);
    }

}