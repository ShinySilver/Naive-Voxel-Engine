//
// Created by silverly on 20/05/2021.
//

#ifndef IVY_GENERATOR_H
#define IVY_GENERATOR_H

#include "../common/entities/entity.h"
#include "../client/utils/meshing/mesh.h"

namespace generator {
    /**
     * Initialize the passed entity allocation using placement new. It will have to be manually destroyed before free.
     *
     * @param cell_coordinate
     * @param entity
     */
    Chunk *generate(const glm::vec3 &cell_coordinate);
}


#endif //IVY_GENERATOR_H
