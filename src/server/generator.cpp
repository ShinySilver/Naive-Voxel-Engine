//
// Created by silverly on 20/05/2021.
//

#include <vector>
#include <glm/glm/gtc/noise.hpp>
#include "generator.h"
#include "../common/world/entities/entity_chunk.h"

#define CHUNK_SIDE 16
#define VOXEL_STONE Color(0.53, 0.55, 0.55)
#define VOXEL_DIRT Color(0.6, 0.46, 0.32)
#define VOXEL_GRASS Color(0.1, 0.6, 0.1)
#define UNDER_CULLING_RATE 2

namespace generator {
    namespace {
        inline int height(int x, int z) {
            return (1 + glm::simplex(glm::vec2(x / 64.0f, z / 64.0f))) * 48;
        }

        inline bool is_empty(const glm::vec3 &c) {
            glm::vec3 ce = c + glm::vec3(CHUNK_SIDE - 1);
            return height(c.x, c.z) > c.y - CHUNK_SIDE * VOXEL_SIZE * UNDER_CULLING_RATE
                   && height(ce.x, c.z) > c.y - CHUNK_SIDE * VOXEL_SIZE * UNDER_CULLING_RATE
                   && height(c.x, ce.z) > c.y - CHUNK_SIDE * VOXEL_SIZE * UNDER_CULLING_RATE
                   && height(ce.x, ce.z) > c.y - CHUNK_SIDE * VOXEL_SIZE * UNDER_CULLING_RATE;
        }
    }


    void generate(const glm::vec3 &cell_coordinate, Entity *entity) {
        // Converting chunk pos to block pos.
        glm::vec<3, int, glm::defaultp> position = cell_coordinate
                                                   * float(DEFAULT_CHUNK_SIDE * VOXEL_SIZE);

        // This chunk's voxel grid
        Voxel voxels[(int) pow(CHUNK_SIDE, 3)];

        // Checking if this is an empty chunk. If yes, go fast and skip it
        if (is_empty(cell_coordinate * (float) CHUNK_SIDE)) {
            for (int dx = 0; dx < CHUNK_SIDE; ++dx) {
                for (int dz = 0; dz < CHUNK_SIDE; ++dz) {
                    int h = height(cell_coordinate.x * CHUNK_SIDE + dx, cell_coordinate.z * CHUNK_SIDE + dz);
                    for (int dy = 0; dy < CHUNK_SIDE; ++dy) {
                        if (cell_coordinate.y * CHUNK_SIDE + dy < h - 5) {
                            voxels[dx + dy * CHUNK_SIDE + dz * CHUNK_SIDE * CHUNK_SIDE] = VOXEL_STONE;
                        } else if (cell_coordinate.y * CHUNK_SIDE + dy < h - 1) {
                            voxels[dx + dy * CHUNK_SIDE + dz * CHUNK_SIDE * CHUNK_SIDE] = VOXEL_DIRT;
                        } else if (cell_coordinate.y * CHUNK_SIDE + dy <= h) {
                            voxels[dx + dy * CHUNK_SIDE + dz * CHUNK_SIDE * CHUNK_SIDE] = VOXEL_GRASS;
                        } else {
                            //voxels[dx + dy * CHUNK_SIDE + dz * CHUNK_SIDE * CHUNK_SIDE] = VOXEL_AIR;
                        }
                    }
                }
            }
        }
        new(entity)
                EntityChunk(Chunk(std::vector<Voxel>(voxels, voxels + (int) pow(CHUNK_SIDE, 3)), CHUNK_SIDE),
                            Location(position)
        );
    }
}