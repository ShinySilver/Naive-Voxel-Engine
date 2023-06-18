//
// Created by silverly on 20/05/2021.
//

#ifndef IVY_WORLD_H
#define IVY_WORLD_H

namespace world {
    /**
     * Initialize the world. Called during server initialization.
     */
    void init();

    /**
     * Tick the world. Called during server tick.
     */
    void tick();
}

#endif //IVY_WORLD_H

