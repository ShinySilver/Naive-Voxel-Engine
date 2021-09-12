#include "world.h"

#include "world/entities/entity_uniform_voxel.h"

namespace world {

static std::vector<Entity*> entities;

void init() {

	//temporary
	for(int i=0; i<3; ++i) {
		auto entity = new EntityUniformVoxel(glm::vec3(2*i, 0, 0), glm::vec4(i/3.0, 0.5, 0.5, 1));
		entities.emplace_back(entity);
	}
}

std::vector<Entity*>& get_entities() {
	return entities;
}

} //namespace world

