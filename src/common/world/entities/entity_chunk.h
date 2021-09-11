/*
 * EntityChunk.h
 *
 *  Created on: 18 juin 2020
 *      Author: silverly
 */

#ifndef WORLD_ENTITY_ENTITYCHUNK_H_
#define WORLD_ENTITY_ENTITYCHUNK_H_

#include <GL/glew.h>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/vec3.hpp>

#include "../../utils/Location.h"
#include "../data/ChunkUtil.h"
#include "entity.h"

/*
 *
 */
class EntityChunk: public IEntity {
public:
	EntityChunk(ChunkUtil::BasicChunk*);
	EntityChunk(ChunkUtil::BasicChunk*, Location);
	virtual ~EntityChunk() = default;

	virtual void preload() override;
	virtual void draw(glm::mat4&) override;
	virtual void unload() override;
private:
	GLuint vertexArrayID = 0, colorArrayID = 0, programID = 0, matrixID = 0,
			vertexBuffer = 0, colorBuffer = 0;
	int verticeBufferSize = 0;

	ChunkUtil::BasicChunk *_chunk;
};

#endif /* WORLD_ENTITY_ENTITYCHUNK_H_ */
