//
// Created by silverly on 20/05/2021.
//

#ifndef IVY_GENERATOR_H
#define IVY_GENERATOR_H

#include "../common/world/entities/entity.h"

namespace generator {
    /**
     * Initialize the passed entity allocation using placement new. It will have to be manually destroyed before free.
     *
     * @param cell_coordinate
     * @param entity
     */
    void generate(const glm::vec3 &cell_coordinate, Entity *entity);
}


#endif //IVY_GENERATOR_H
