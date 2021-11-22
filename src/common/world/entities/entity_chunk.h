/*
 * EntityChunk.h
 *
 *  Created on: 18 juin 2020
 *      Author: silverly
 */

#ifndef WORLD_ENTITY_ENTITYCHUNK_H_
#define WORLD_ENTITY_ENTITYCHUNK_H_

#ifndef __gl_h_
#include <glad/glad.h>
#endif

#include "entity.h"
#include "../../../client/utils/mesher/chunk_util.h"

/*
 *
 */
class EntityChunk: public Entity {
public:
	EntityChunk(ChunkUtil::BasicChunk);
	EntityChunk(ChunkUtil::BasicChunk, Location);
	virtual ~EntityChunk() = default;

	virtual void preload() override;
	virtual void draw(glm::mat4&, const glm::vec3& light_dir, const glm::vec3& view_pos) override;
	virtual void unload() override;
private:
	GLuint vertexArrayID = 0, 
		   colorArrayID = 0, 
		   programID = 0, 
		   matrix_ID = 0,
		   normal_mat_ID = 0,
		   light_ID = 0,
		   view_ID = 0,
		   vertexBuffer = 0, 
		   colorBuffer = 0,
		   normalBuffer = 0;
	int verticeBufferSize = 0;

	ChunkUtil::BasicChunk _chunk;
};

#endif /* WORLD_ENTITY_ENTITYCHUNK_H_ */
