/*
 * EntityVoxel.h
 *
 *  Created on: 30 avr. 2020
 *      Author: silverly
 */

#ifndef WORLD_ENTITY_ENTITYVOXEL_H_
#define WORLD_ENTITY_ENTITYVOXEL_H_

#include <GL/glew.h>

#include "../../../../libs/glm/glm/ext/matrix_float4x4.hpp"
#include "../IEntity.h"

/*
 *
 */
class EntityVoxel: public IEntity {
	using IEntity::IEntity;
	virtual void preload() override;
	virtual void draw(glm::mat4&) override;
	virtual void unload() override;
private:
	GLuint vertexArrayID, programID, vertexBuffer, uvBuffer, matrixID, texture,
			textureID;
	int verticeBufferSize;
};

#endif /* WORLD_ENTITY_ENTITYVOXEL_H_ */
