/*
 * EntityVoxel.h
 *
 *  Created on: 30 avr. 2020
 *      Author: silverly
 */

#ifndef WORLD_ENTITY_ENTITYVOXEL_H_
#define WORLD_ENTITY_ENTITYVOXEL_H_

#ifndef __gl_h_

#include <glad/glad.h>

#endif

#include "entity.h"

/*
 *
 */
class EntityVoxel : public Entity {
    using Entity::Entity;

    virtual void preload() override;

    virtual void load() override;

    virtual void draw(glm::mat4 &) override;

    virtual void unload() override;

private:
    GLuint vertexArrayID, programID, vertexBuffer, uvBuffer, matrixID, texture,
            textureID;
    int verticeBufferSize;
};

#endif /* WORLD_ENTITY_ENTITYVOXEL_H_ */
