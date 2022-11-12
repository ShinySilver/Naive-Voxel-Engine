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
class EntityChunk : public Entity {
public:
    EntityChunk(Chunk);

    EntityChunk(Chunk, Location);

    virtual ~EntityChunk() = default;

	virtual void preload() override;
	virtual void load() override;
	virtual void draw(glm::mat4&, const glm::vec3& light_dir, const glm::vec3& view_pos) override;
	virtual void unload() override;

private:
	static GLuint program_ID, 
		   matrix_ID,
		   normal_mat_ID,
		   light_ID,
		   view_ID;

	GLuint vertexArrayID = 0, 
		   colorArrayID = 0, 
		   vertexBuffer = 0, 
		   colorBuffer = 0,
		   normalBuffer = 0;

	int verticeBufferSize = 0;

    Chunk _chunk;
    // TODO: Fill them up during the chunk loading.
    // TODO: Maybe we could replace the chunk buffer with a linked list using these.
    // TODO: Merge together Chunks & chunk entities?
    Chunk *adjacent_chunks[6];
    ChunkUtil::Mesh *_mesh;
};

#endif /* WORLD_ENTITY_ENTITYCHUNK_H_ */

