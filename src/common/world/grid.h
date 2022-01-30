//
// Created by silverly on 14/09/2021.
//

#ifndef IVY_GRID_H
#define IVY_GRID_H

#include "chunk.h"
#include "../../client/utils/mesher/chunk_util.h"

namespace grid {
    inline glm::vec3 pos_to_chunk(const Location &v){
        return glm::vec3(int(v.position.x/DEFAULT_CHUNK_SIDE/VOXEL_SIZE),
                         int(v.position.y/DEFAULT_CHUNK_SIDE/VOXEL_SIZE),
						 0);
    }
}


#endif //IVY_GRID_H
