#include "world.h"
#include "chunk.h"
#include "../../client/utils/mesher/chunk_util.h"
#include "entities/entity_chunk.h"

#include <cmath>

#define CHUNK_SIDE 16

#define VOXEL_STONE Color(0.8, 0.8, 0.8)
#define VOXEL_GRASS Color(0.1, 0.6, 0.1)
#define VOXEL_AIR Color(0.0, 0.0, 0.0)

#define TICK_RANGE 64
#define VIEW_RANGE 128

namespace world {
    namespace {
        static std::vector<Entity *> entities;

        std::vector<Entity *> generate_island(uint32_t sea_level, uint32_t height,
                                              uint32_t depth) {

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

            auto chunk_entity = new EntityChunk(Chunk(std::move(voxels), CHUNK_SIDE));
            chunk_entity->preload();

            return std::move(std::vector<Entity *>{chunk_entity});
        }
    }


    void init() {

        auto island = generate_island(7, 8, 6);
        entities.reserve(entities.size() + island.size());

        for (auto entity : island) {
            entities.emplace_back(entity);
        }
    }

    std::vector<Entity *> &get_entities() {
        return entities;
    }

    std::vector<Entity *> get_cell(glm::vec3 &cell_coordinate) {
        return generate_island(7, 8, 6);
    }

    void get_cell_async(glm::vec3 &cell_coordinate, std::function<void(const std::vector<Entity *> &cell)> callback) {
        callback(generate_island(7, 8, 6));
    }

} //namespace world

