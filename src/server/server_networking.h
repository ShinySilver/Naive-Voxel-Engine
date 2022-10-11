//
// Created by silver on 11/10/22.
//

#ifndef IVY_CLIENT_NETWORKING_H
#define IVY_CLIENT_NETWORKING_H


#include "../common/world/entities/entity.h"

namespace server_networking {
    /**
     * Initialize the port listener & ready the workers for applying client requests on the world and world generation.
     * Half of the workers prioritize world generation, the other prefer applying client requests on the world.
     */
    void init();

    /**
     * Finish the tasks currently being worked on, close all sockets and free dynamic resources.
     */
    void stop();

    /**
     * Wait for the workers & networking stack to exit.
     */
    void join();

    /**
     * Apply for a modification of the given chunk
     */
    void queue_chunk_request();

    /**
     * Apply for some world generation
     */
    void queue_chunk_generation_request(const glm::vec3 &cell_coordinate, Entity *entity);
};


#endif //IVY_CLIENT_NETWORKING_H
