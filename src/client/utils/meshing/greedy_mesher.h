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

#define VOXEL_SIZE (1.0f)
#define CHUNK_WIDTH  CHUNK_DATA_SIZE
#define CHUNK_HEIGHT CHUNK_DATA_SIZE
#define MESH_CHUNK_BORDER false

/*
 *
 */
namespace GreedyMesher {

    Mesh* mesh(Chunk& chunk, Chunk *neighbours);

} //namespace GreedyMesher

#endif /* WORLD_DATA_CHUNKUTIL_H_ */
