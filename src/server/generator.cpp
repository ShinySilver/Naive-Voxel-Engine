//
// Created by silverly on 20/05/2021.
//

#include <vector>
#include "generator.h"
#include "../common/world/entities/entity_chunk.h"

#define CHUNK_SIDE 16
#define VOXEL_STONE Color(0.8, 0.8, 0.8)
#define VOXEL_GRASS Color(0.1, 0.6, 0.1)

namespace generator {
    namespace {
        void generate_island(Entity *entity, uint32_t sea_level, uint32_t height,
                             uint32_t depth, glm::vec3 position) {

            //generating upper enveloppe
            std::vector<char> upper_env;
            upper_env.reserve(std::pow(CHUNK_SIDE, 2));
            height += 1;

            for (int i = 0; i < CHUNK_SIDE; ++i) {
                for (int j = 0; j < CHUNK_SIDE; ++j) {
                    upper_env.emplace_back(sea_level - 1
                                           + height
                                             * 2 / (1 + std::exp(0.5 * std::abs(i - CHUNK_SIDE / 2)))
                                             * 2 / (1 + std::exp(0.5 * std::abs(j - CHUNK_SIDE / 2))));
                }
            }

            //generating lower enveloppe
            std::vector<char> lower_env;
            lower_env.reserve(std::pow(CHUNK_SIDE, 2));
            depth += 2;

            for (int i = 0; i < CHUNK_SIDE; ++i) {
                for (int j = 0; j < CHUNK_SIDE; ++j) {
                    lower_env.emplace_back(sea_level
                                           - depth
                                             * 2 / (1 + std::exp(0.4 * std::abs(i - CHUNK_SIDE / 2)))
                                             * 2 / (1 + std::exp(0.4 * std::abs(j - CHUNK_SIDE / 2))));
                }
            }

            std::vector<Voxel> voxels;
            voxels.reserve(std::pow(CHUNK_SIDE, 3));
            for (int i = 0; i < CHUNK_SIDE; ++i) {
                for (int j = 0; j < CHUNK_SIDE; ++j) {
                    int max = 0;
                    for (int k = 0; k < CHUNK_SIDE; ++k) {
                        if (k < upper_env[i + j * CHUNK_SIDE]
                            && k > lower_env[i + j * CHUNK_SIDE]) {
                            voxels.emplace_back(Voxel(VOXEL_STONE));
                            max = k;
                        } else {
                            voxels.emplace_back();
                        }
                    }
                    if (max) {
                        voxels[i * std::pow(CHUNK_SIDE, 2) + j * CHUNK_SIDE + max]
                                = Voxel(VOXEL_GRASS);
                    }
                }
            }

            new(entity) EntityChunk(Chunk(std::move(voxels), CHUNK_SIDE), Location(position));
        }
    }

    void generate(const glm::vec3 &cell_coordinate, Entity *entity) {
        generate_island(entity, 7, 8, 6, cell_coordinate
                                         * float(DEFAULT_CHUNK_SIDE * VOXEL_SIZE));
    }
}