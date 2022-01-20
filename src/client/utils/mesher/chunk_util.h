/*
 * ChunkUtil.h
 *
 *  Created on: 5 mai 2020
 *      Author: silverly
 */

#ifndef WORLD_DATA_CHUNKUTIL_H_
#define WORLD_DATA_CHUNKUTIL_H_

#include <vector>

#include <glm/glm/vec3.hpp>
#include "../../../common/world/chunk.h"

#define VOXEL_SIZE (2.0f)
#define CHUNK_WIDTH  (16)
#define CHUNK_HEIGHT (16)

/*
 *
 */
namespace ChunkUtil {
enum MesherType {
	GREEDY, NAIVE_SURFACE_NET, NAIVE_WITH_CULLING
};

typedef struct {
	std::vector<glm::vec3> vertices;
	std::vector<Color> colors;
	std::vector<glm::vec3> normals;
} Mesh;

Mesh* naiveSurfaceNetsMesh(Chunk &chunk);
Mesh* greedyMesh(Chunk& chunk);
Mesh* naiveMeshWithCulling(Chunk &chunk);

void generateChunkMesh(Chunk &chunk, MesherType type);
}

#endif /* WORLD_DATA_CHUNKUTIL_H_ */
