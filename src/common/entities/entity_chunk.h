/*
 * EntityChunk.h
 *
 *  Created on: 18 juin 2020
 *      Author: silverly
 */

#ifndef WORLD_ENTITY_ENTITYCHUNK_H_
#define WORLD_ENTITY_ENTITYCHUNK_H_

#ifndef __gl_h_
#include "glad/glad.h"
#endif

#include "entity.h"
#include "../../client/utils/meshing/mesh.h"


class EntityChunk : public Entity {
public:
    EntityChunk(Mesh *);

    EntityChunk(Mesh *, Location);

    virtual ~EntityChunk() = default;

    virtual void preload() override;

    virtual void load() override;

    virtual void draw(glm::mat4 &, const glm::vec3 &light_dir, const glm::vec3 &view_pos) override;

    virtual void unload() override;

private:
    static GLuint program_ID,
            matrix_ID,
            normal_mat_ID,
            light_ID,
            view_ID;
    static int active_instances;

    GLuint vertexArrayID = 0,
            vertexBuffer = 0,
            colorBuffer = 0,
            normalBuffer = 0;

    int verticeBufferSize = 0;
    Chunk *adjacent_chunks[6];
    Mesh *_mesh;
    glm::mat3 _normal_matrix;
};

#endif /* WORLD_ENTITY_ENTITYCHUNK_H_ */

