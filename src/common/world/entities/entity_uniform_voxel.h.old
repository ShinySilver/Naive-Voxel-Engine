/*
 * EntityUniformVoxel.h
 *
 *  Created on: 30 avr. 2020
 *      Author: silverly
 */

#ifndef WORLD_ENTITY_ENTITYUNIFORMVOXEL_H_
#define WORLD_ENTITY_ENTITYUNIFORMVOXEL_H_

#include <glm/glm/vec3.hpp>
#include "entity.h"

/*
 *
 */
class EntityUniformVoxel : public Entity {
public:
    EntityUniformVoxel(const glm::vec3 &position, const glm::vec4 &color);

    EntityUniformVoxel(const glm::vec3 &position, const glm::vec3 &color);

    using Entity::Entity;

    virtual void preload() override;

    virtual void load() override;

    virtual void draw(glm::mat4 &) override;

    virtual void unload() override;

private:
    glm::vec4 _color = glm::vec4{0, 1, 0, 0};
};

#endif /* WORLD_ENTITY_ENTITYUNIFORMVOXEL_H_ */
