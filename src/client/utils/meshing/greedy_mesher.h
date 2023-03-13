/*
 * GreedyMesher.h
 *
 *  Created on: 5 mai 2020
 *      Author: silverly
 */

#ifndef WORLD_DATA_CHUNKUTIL_H_
#define WORLD_DATA_CHUNKUTIL_H_

#include <vector>

#include <glm/glm/vec3.hpp>
#include "mesh.h"

#define VOXEL_SIZE (2.0f)
#define CHUNK_WIDTH  CHUNK_SIZE
#define CHUNK_HEIGHT CHUNK_SIZE

/*
 *
 */
namespace GreedyMesher {

    Mesh* mesh(Chunk& chunk);

} //namespace GreedyMesher

#endif /* WORLD_DATA_CHUNKUTIL_H_ */
