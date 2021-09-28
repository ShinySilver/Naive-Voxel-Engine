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

/*
 *
 */
namespace ChunkUtil {
enum MesherType {
	GREEDY, NAIVE_SURFACE_NET, NAIVE_WITH_CULLING
};

typedef glm::vec3 Color;

typedef struct {
	std::vector<glm::vec3> vertices;
	std::vector<Color> colors;
	std::vector<glm::vec3> normals;
} Mesh;

typedef struct VoxelFace {
	bool transparent; // TODO: replace voxelface with vec4 colors
	int side;
	Color color;

	VoxelFace(bool transparent, int side, Color color): transparent(transparent),
			side(side), color(color){
	}

	inline bool equals(ChunkUtil::VoxelFace face) {
		return face.transparent == this->transparent
				&& face.color == this->color;
	}
	inline bool equals(ChunkUtil::VoxelFace *face) {
		return face->transparent == this->transparent
				&& face->color == this->color;
	}
} VoxelFace;

typedef struct Voxel {
	Color color;
	VoxelFace uniformFace;

	Voxel(Color color) :
		color(color), uniformFace { false, 0, color } {
	}

	Voxel() :
		color{0,128,0}, uniformFace { true, 0, {0,128,0} } {
	}

	inline bool isEmpty() {
		return color.r == 0 && color.g == 0 && color.b == 0;
	}
} Voxel;

typedef struct {
	std::vector<Voxel> data;
	short size; 				//side size (the chunk is cubic)
	inline Voxel& get(int x, int y, int z) {
		return data[x + size * y + size * size * z];
	}
} BasicChunk;

Mesh* naiveSurfaceNetsMesh(BasicChunk &chunk);
Mesh* greedyMesh(BasicChunk& chunk);
Mesh* naiveMeshWithCulling(BasicChunk &chunk);

void generateChunkMesh(BasicChunk &chunk, MesherType type);
}

#endif /* WORLD_DATA_CHUNKUTIL_H_ */
