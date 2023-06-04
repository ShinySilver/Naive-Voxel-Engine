//
// Created by silverly on 20/05/2021.
//

#include <vector>
#include <cfloat>
#include "glm/glm/gtc/noise.hpp"
#include "generator.h"
#include "../../client/utils/meshing/greedy_mesher.h"
#include "../../common/world/voxels.h"

#define SIMPLEX_1_WIDTH 256.0f
#define SIMPLEX_1_HEIGHT 128

#define CONTINENT_NOISE_WIDTH 2048.0f
#define CONTINENT_NOISE_HEIGHT 128
#define CONTINENT_MIN_HEIGHT 7
#define CONTINENT_THRESHOLD 224
#define CONTINENT_TRANSITION 4
#define CONTINENT_TRANSITION_HEIGHT 3

#define SIMPLEX_3_WIDTH 256.0f
#define SIMPLEX_3_HEIGHT 128

#define SIMPLEX_4_WIDTH 256.0f
#define SIMPLEX_4_HEIGHT 128

#define SIMPLEX_5_WIDTH 256.0f
#define SIMPLEX_5_HEIGHT 128

#define TERRAIN_OFFSET (-64)

// Ou 64 48

namespace generator {
    namespace {
        inline float is_continent(float x, float z) {
            float value = (1 + glm::simplex(glm::vec2(x / CONTINENT_NOISE_WIDTH, z / CONTINENT_NOISE_WIDTH))) *
                          CONTINENT_NOISE_HEIGHT - CONTINENT_THRESHOLD;
            if (value <= 0) {
                return FLT_MIN;
            } else if (value < CONTINENT_TRANSITION) {
                value -= (CONTINENT_TRANSITION - value) / CONTINENT_TRANSITION *
                         CONTINENT_TRANSITION_HEIGHT;
            }
            return value + CONTINENT_MIN_HEIGHT / 2;
        }

        inline float height(float x, float z) {
            return is_continent(x, z);
        }

        inline float is_mountain(float x, float z) {
            return (1 + glm::simplex(glm::vec2(x / SIMPLEX_3_WIDTH, z / SIMPLEX_3_WIDTH))) * SIMPLEX_3_HEIGHT +
                   TERRAIN_OFFSET;
        }

        inline float noise_mountains(float x, float z) {
            return (1 + glm::simplex(glm::vec2(x / SIMPLEX_4_WIDTH, z / SIMPLEX_4_WIDTH))) * SIMPLEX_4_HEIGHT +
                   TERRAIN_OFFSET;
        }

        inline float noise_plains(float x, float z) {
            return (1 + glm::simplex(glm::vec2(x / SIMPLEX_5_WIDTH, z / SIMPLEX_5_WIDTH))) * SIMPLEX_5_HEIGHT +
                   TERRAIN_OFFSET;
        }
    }


    Chunk *generate(const glm::vec3 &cell_coordinate) {
        // Converting chunk pos to block pos.
        glm::vec<3, int, glm::defaultp> position = cell_coordinate
                                                   * float(CHUNK_SIZE * VOXEL_SIZE);

        Chunk *chunk = nullptr;
        for (int dx = 0; dx <= CHUNK_SIZE; ++dx) {
            for (int dz = 0; dz <= CHUNK_SIZE; ++dz) {
                float h_max = height(cell_coordinate.x * CHUNK_SIZE + dx, cell_coordinate.z * CHUNK_SIZE + dz);
                if (h_max == FLT_MIN) continue;
                float h_min = -h_max + TERRAIN_OFFSET - CONTINENT_MIN_HEIGHT;
                h_min = std::max(h_min, (cell_coordinate.y) * CHUNK_SIZE);
                h_max = std::min(h_max + TERRAIN_OFFSET, (cell_coordinate.y + 1) * CHUNK_SIZE);
                if (h_max > cell_coordinate.y * CHUNK_SIZE && h_min < (cell_coordinate.y + 1) * CHUNK_SIZE &&
                    chunk == nullptr) {
                    chunk = new Chunk();
                }
                for (int dy = h_min - cell_coordinate.y * CHUNK_SIZE;
                     dy <= h_max - cell_coordinate.y * CHUNK_SIZE; ++dy) {
                    if (cell_coordinate.y * CHUNK_SIZE + dy < h_max - 5) {
                        chunk->set(dx, dy, dz, VOXEL_STONE);
                    } else if (cell_coordinate.y * CHUNK_SIZE + dy < h_max - 1) {
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