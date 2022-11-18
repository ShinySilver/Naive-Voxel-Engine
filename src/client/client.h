//
// Created by silverly on 20/05/2021.
//

#ifndef IVY_CLIENT_H
#define IVY_CLIENT_H

#include "../common/world/entities/entity.h"


#define VIEW_DISTANCE 8
#define TICK_DISTANCE 4
#define CLIENT_SECONDARY_WORKER_THREAD_NUMBER 6

namespace client {
    void tick();
}

#endif //IVY_CLIENT_H

