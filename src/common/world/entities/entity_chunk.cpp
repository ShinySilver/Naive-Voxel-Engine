/*
 * EntityChunk.cpp
 *
 *  Created on: 18 juin 2020
 *      Author: silverly
 */

#include "entity_chunk.h"

#include "../../../client/utils/loaders/obj_loader.h"
#include "../../../client/utils/shader/shader_base.h"
#include "../../../client/utils/loaders/shader_loader.h"
#include "../../utils/location.h"
#include "../../../client/utils/mesher/chunk_util.h"
#include "../grid.h"

#include <glm/glm/vec3.hpp>
#include <glm/glm/ext/matrix_float4x4.hpp>
#include <loguru.hpp>

#include <vector>

GLuint EntityChunk::program_ID = 0;
GLuint EntityChunk::matrix_ID = 0;
GLuint EntityChunk::normal_mat_ID = 0;
GLuint EntityChunk::light_ID = 0;
GLuint EntityChunk::view_ID = 0;

EntityChunk::EntityChunk(Chunk chunk) :
        _chunk(std::move(chunk)) {}

EntityChunk::EntityChunk(Chunk chunk, Location loc) :
        Entity(loc), _chunk(std::move(chunk)) {}

void EntityChunk::preload() {
    //std::cout << "Preloading Chunk.\n";

    // Read our .obj file
    //std::vector<glm::vec3> vertices;
    //std::vector<glm::vec3> colors;
    //loadOBJ("resources/models/example/cube.obj", vertices, uvs, normals);
    //glm::vec3 chunk_pos = grid::pos_to_chunk(getLocation());
    //std::cout << "Meshing chunk at " << int(chunk_pos.x) << "; "
    //          << int(chunk_pos.y) << "; "
    //          << int(chunk_pos.z) << "...\n";
    _mesh = ChunkUtil::greedyMesh(_chunk);
}

void EntityChunk::load() {
    //std::cout << "Loading Chunk.\n";

    // Our VAO
    glGenVertexArrays(1, &vertexArrayID);
    glBindVertexArray(vertexArrayID);

    if (!program_ID) {
        // The 1st time, create and compile our GLSL program from the shaders
        LOG_S(1) << "Loading shaders...";
        program_ID = LoadShaders("resources/shaders/chunkColor/chunkColor.vert",
                                 "resources/shaders/chunkColor/chunkColor.frag");

        // Get a handle for our "MVP" uniform
        matrix_ID = glGetUniformLocation(program_ID, "MVP");
		normal_mat_ID = glGetUniformLocation(program_ID, "normal_mat");
		light_ID = glGetUniformLocation(program_ID, "light_position");
		view_ID = glGetUniformLocation(program_ID, "view_position");
    }

    //std::cout << "Creating VAB...\n";
    // First attribute of our VAO, vertices
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, _mesh->vertices.size() * sizeof(glm::vec3),
                 _mesh->vertices.data(), GL_STATIC_DRAW);
    verticeBufferSize = (int) _mesh->vertices.size();
    //std::cout << "Vertice Buffer Size: " << verticeBufferSize << "\n";

    // 2nd attribute, colors
    glGenBuffers(1, &colorBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
    glBufferData(GL_ARRAY_BUFFER, _mesh->colors.size() * sizeof(glm::vec3),
                 _mesh->colors.data(), GL_STATIC_DRAW);

	// 3rd attribute, normals
	glGenBuffers(1, &normalBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
	glBufferData(GL_ARRAY_BUFFER, _mesh->normals.size() * sizeof(glm::vec3),
				_mesh->normals.data(), GL_STATIC_DRAW);

    delete _mesh;
    _is_loaded = true;
    //std::cout << "Done preloading Chunk.\n";
}

void EntityChunk::draw(glm::mat4 &base, const glm::vec3& light_pos, const glm::vec3& view_pos) {

	// shader program
	glUseProgram(program_ID);

	// uniforms
	auto MVP = glm::rotate( glm::rotate( glm::rotate(
					glm::translate(base,_extraPosition), 
					_extraRotation.x, glm::vec3(1.0f,0.0f, 0.0f)), 
				_extraRotation.y, glm::vec3(0.0f,1.0f, 0.0f)), 
			_extraRotation.z, glm::vec3(0.0f,0.0f, 1.0f));

	glUniformMatrix4fv(matrix_ID, 1, GL_FALSE, glm::value_ptr(MVP));
	glUniformMatrix3fv(normal_mat_ID, 1, GL_FALSE, glm::value_ptr(_normal_matrix));
	glUniform3fv(light_ID, 1, glm::value_ptr(light_pos));
	glUniform3fv(view_ID, 1, glm::value_ptr(view_pos));

	// vertex array
	glBindVertexArray(vertexArrayID);

    // 1rst attribute buffer : vertices
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glVertexAttribPointer(0,    // attribute
                          3,    // size
                          GL_FLOAT,    // type
                          GL_FALSE,    // normalized?
                          0,    // stride
                          (void *) 0    // array buffer offset
    );

    // 2nd attribute buffer : colors
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
    glVertexAttribPointer(1,    // attribute
                          3,    // size 3, rgb
                          GL_FLOAT,    // type
                          GL_FALSE,    // normalized?
                          0,    // stride
                          (void *) 0    // array buffer offset
    );

	// 3rd attribute buffer : normals
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
	glVertexAttribPointer(2, 
			3,
			GL_FLOAT,
			GL_FALSE,
			0,
			(void*) 0);

	// Draw the triangles !
	glDrawArrays(GL_TRIANGLES, 0, verticeBufferSize);

	// And then we unbind everything
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glBindVertexArray(0);
}

void EntityChunk::unload() {
	glDeleteVertexArrays(1, &vertexArrayID);
	glDeleteProgram(program_ID);
	glDeleteBuffers(1, &vertexBuffer);
	glDeleteBuffers(1, &colorBuffer);
	glDeleteBuffers(1, &normalBuffer);
}

