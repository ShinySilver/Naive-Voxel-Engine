//
// Created by silverly on 20/05/2021.
//

#include <vector>
#include <glm/glm/gtc/noise.hpp>
#include "generator.h"
#include "../client/utils/meshing/greedy_mesher.h"
#include "../common/world/chunk.h"
#include "../common/world/voxels.h"
#include "../common/entities/entity_chunk.h"

#define UNDER_CULLING_RATE 1
#define SIMPLEX_1_WIDTH 256.0f
#define SIMPLEX_1_HEIGHT 128
#define TERRAIN_OFFSET (-256)

// Ou 64 48

namespace generator {
    namespace {
        inline int height(float x, float z) {
            return (1 + glm::simplex(glm::vec2(x / SIMPLEX_1_WIDTH, z / SIMPLEX_1_WIDTH))) * SIMPLEX_1_HEIGHT +
                   TERRAIN_OFFSET;
        }
    }


    Chunk *generate(const glm::vec3 &cell_coordinate) {
        // Converting chunk pos to block pos.
        glm::vec<3, int, glm::defaultp> position = cell_coordinate
                                                   * float(CHUNK_SIZE * VOXEL_SIZE);

        Chunk *chunk = nullptr;
        for (int dx = 0; dx < CHUNK_SIZE; ++dx) {
            for (int dz = 0; dz < CHUNK_SIZE; ++dz) {
                int h = std::min(height(cell_coordinate.x * CHUNK_SIZE + dx, cell_coordinate.z * CHUNK_SIZE + dz),
                                 int((cell_coordinate.y + 1) * CHUNK_SIZE));
                if (h > cell_coordinate.y * CHUNK_SIZE && chunk == nullptr) {
                    chunk = new Chunk();
                }
                for (int dy = 0; dy < h - cell_coordinate.y * CHUNK_SIZE; ++dy) {
                    if (cell_coordinate.y * CHUNK_SIZE + dy < h - 5) {
                        chunk->set(dx, dy, dz, VOXEL_STONE);
                    } else if (cell_coordinate.y * CHUNK_SIZE + dy < h - 1) {
                        chunk->set(dx, dy, dz, VOXEL_DIRT);
                    } else {
                        chunk->set(dx, dy, dz, VOXEL_GRASS);
                    }
                }
            }
        }
        return chunk;
    }
}