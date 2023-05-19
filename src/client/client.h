//
// Created by silverly on 20/05/2021.
//

#ifndef IVY_CLIENT_H
#define IVY_CLIENT_H


#define VIEW_DISTANCE 12
#define TICK_DISTANCE 4
#define CLIENT_SECONDARY_WORKER_THREAD_NUMBER 6
#define KEEP_CHUNKS_DATA_IN_MEMORY false
#define MAX_CHUNK_LOADING_QUEUE_SIZE 1000
#define MINECRAFT_LIKE_CAMERA true

namespace client {
    void tick();
}

#endif //IVY_CLIENT_H

