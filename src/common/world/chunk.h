//
// Created by silver on 18/01/23.
//

#ifndef IVY_CHUNK_H
#define IVY_CHUNK_H

#include "voxel.h"
#include "glm/glm/vec3.hpp"

#define CHUNK_SIZE (64)
#define CHUNK_SIZE_CUBED (CHUNK_SIZE*CHUNK_SIZE*CHUNK_SIZE)


struct Chunk {
public:
    inline Chunk() : data{} {}
    inline ~Chunk(){}

    inline Voxel::Voxel get(int x, int y, int z) const {
	{
		return data[x + CHUNK_SIZE * y + CHUNK_SIZE * CHUNK_SIZE * z];}
	}

    inline void set(int x, int y, int z, Voxel::Voxel v)
	{
		data[x + CHUNK_SIZE * y + CHUNK_SIZE * CHUNK_SIZE * z] = v;
	}

	Voxel::Voxel data[CHUNK_SIZE_CUBED];

    bool has_full_face(glm::vec3 direction) {
        return false;
    }
};

#endif //IVY_CHUNK_H
