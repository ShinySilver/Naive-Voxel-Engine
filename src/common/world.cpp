#include "world.h"

#include "../client/utils/mesher/chunk_util.h"
#include "world/entities/entity_chunk.h"
#include "world/entities/entity_uniform_voxel.h"

#include <cmath>

#define CHUNK_SIDE 16
#define VOXEL_STONE ChunkUtil::Color(0.8, 0.8, 0.8)
#define VOXEL_AIR ChunkUtil::Color(0.0, 0.0, 0.0)

//#define maron 0.6, 0.4, 0.2
//#define vert 0.1, 0.6, 0.1
//#define vert_petan 0.1, 0.9, 0.0
//#define terre 0.8, 0.5, 0.3

namespace world {

static std::vector<Entity*> entities;

std::vector<Entity*> generate_island();

void init() {

	auto island = generate_island();
	entities.reserve(entities.size() + island.size());

	for(auto entity : std::move(island)) {
		entities.emplace_back(entity);
	}
}

std::vector<Entity*>& get_entities() {
	return entities;
}

std::vector<Entity*> generate_island() {

	//generating upper enveloppe
	std::vector<char> upper_env;
	upper_env.reserve(std::pow(CHUNK_SIDE, 2));

	for(int i=0; i<CHUNK_SIDE; ++i) {
		for(int j=0; j<CHUNK_SIDE; ++j) {
			upper_env.emplace_back(CHUNK_SIDE/2.0 
					+ CHUNK_SIDE/2.0 * std::sin(i/16.0*M_PI) * std::cos(j/16.0*M_PI));
		}
	}

	//generating lower enveloppe
	std::vector<char> lower_env;
	lower_env.reserve(std::pow(CHUNK_SIDE, 2));
	
	for(int i=0; i<CHUNK_SIDE; ++i) {
		for(int j=0; j<CHUNK_SIDE; ++j) {
			upper_env.emplace_back(CHUNK_SIDE/2.0 
					- CHUNK_SIDE/2.0 * std::sin(i/16.0*M_PI) * std::cos(j/16.0*M_PI));
		}
	}

	std::vector<ChunkUtil::Voxel> voxels;
	voxels.reserve(std::pow(CHUNK_SIDE, 3));
	for(int i=0 ; i<CHUNK_SIDE; ++i) {
		for(int j=0; j<CHUNK_SIDE; ++j) {
			for(int k=0; k<CHUNK_SIDE; ++k) {
				if(k < upper_env[i + j*CHUNK_SIDE] && k > lower_env[i + j*CHUNK_SIDE]) {
					voxels.emplace_back(ChunkUtil::Voxel(VOXEL_STONE));
				} else {
					voxels.emplace_back(ChunkUtil::Voxel(VOXEL_AIR));
				}
			}
		}
	}

	auto chunk_entity = new EntityChunk(ChunkUtil::BasicChunk{std::move(voxels), CHUNK_SIDE});

	return {chunk_entity};
}

} //namespace world

