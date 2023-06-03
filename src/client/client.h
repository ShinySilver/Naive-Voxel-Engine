//
// Created by silverly on 20/05/2021.
//

#ifndef IVY_CLIENT_H
#define IVY_CLIENT_H


#include "glm/glm/ext/matrix_float4x4.hpp"
#include "utils/shader/text_renderer.h"

#define VIEW_DISTANCE 6
#define TICK_DISTANCE 4
#define CLIENT_SECONDARY_WORKER_THREAD_NUMBER 10
#define KEEP_CHUNKS_DATA_IN_MEMORY false
#define MINECRAFT_LIKE_CAMERA true
#define ALLOW_DEBUG_STATS true

namespace client {
    void tick();
}

#endif //IVY_CLIENT_H

