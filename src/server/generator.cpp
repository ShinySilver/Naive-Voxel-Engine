//
// Created by silverly on 20/05/2021.
//

#include <vector>
#include <glm/glm/gtc/noise.hpp>
#include "generator.h"
#include "../client/utils/meshing/greedy_mesher.h"
#include "../common/world/chunk.h"
#include "../common/entities/entity_chunk.h"

#define VOXEL_STONE Voxel(0.53, 0.55, 0.55)
#define VOXEL_DIRT Voxel(0.6, 0.46, 0.32)
#define VOXEL_GRASS Voxel(0.1, 0.6, 0.1)
#define VOXEL_AIR Voxel(0.0, 0.0, 0.0)
#define UNDER_CULLING_RATE 2

namespace generator {
    namespace {
        inline int height(int x, int z) {
            return (1 + glm::simplex(glm::vec2(x / 64.0f, z / 64.0f))) * 48;
        }

        inline bool is_empty(const glm::vec3 &c) {
            glm::vec3 ce = c + glm::vec3(CHUNK_SIZE - 1);
            return height(c.x, c.z) > c.y - CHUNK_SIZE * VOXEL_SIZE * UNDER_CULLING_RATE
                   && height(ce.x, c.z) > c.y - CHUNK_SIZE * VOXEL_SIZE * UNDER_CULLING_RATE
                   && height(c.x, ce.z) > c.y - CHUNK_SIZE * VOXEL_SIZE * UNDER_CULLING_RATE
                   && height(ce.x, ce.z) > c.y - CHUNK_SIZE * VOXEL_SIZE * UNDER_CULLING_RATE;
        }
    }


    Chunk *generate(const glm::vec3 &cell_coordinate) {
        // Converting chunk pos to block pos.
        glm::vec<3, int, glm::defaultp> position = cell_coordinate
                                                   * float(CHUNK_SIZE * VOXEL_SIZE);

        // Checking if this is an empty chunk. If yes, go fast and skip it
        if (is_empty(cell_coordinate * (float) CHUNK_SIZE)) {
            Voxel *data = (Voxel *) malloc(sizeof(Voxel) * CHUNK_SIZE_CUBED);
            Chunk *chunk = new Chunk(data);
            for (int dx = 0; dx < CHUNK_SIZE; ++dx) {
                for (int dz = 0; dz < CHUNK_SIZE; ++dz) {
                    int h = height(cell_coordinate.x * CHUNK_SIZE + dx, cell_coordinate.z * CHUNK_SIZE + dz);
                    for (int dy = 0; dy < CHUNK_SIZE; ++dy) {
                        if (cell_coordinate.y * CHUNK_SIZE + dy < h - 5) {
                            chunk->set(dx, dy, dz, VOXEL_STONE);
                        } else if (cell_coordinate.y * CHUNK_SIZE + dy < h - 1) {
                            chunk->set(dx, dy, dz, VOXEL_DIRT);
                        } else if (cell_coordinate.y * CHUNK_SIZE + dy <= h) {
                            chunk->set(dx, dy, dz, VOXEL_GRASS);
                        } else {
                            chunk->set(dx, dy, dz, VOXEL_AIR);
                        }
                    }
                }
            }
            return chunk;
        }
        return nullptr;
    }
}