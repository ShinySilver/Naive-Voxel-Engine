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

#include <iostream>
#include <vector>
#include <glm/glm/vec3.hpp>
#include <glm/glm/ext/matrix_float4x4.hpp>

EntityChunk::EntityChunk(ChunkUtil::BasicChunk chunk) :
		_chunk(std::move(chunk)) {}

EntityChunk::EntityChunk(ChunkUtil::BasicChunk chunk, Location loc) :
		Entity(loc), _chunk(std::move(chunk)) {}

void EntityChunk::preload() {
	std::cout << "Preloading Chunk.\n";

	// Our VAO
	glGenVertexArrays(1, &vertexArrayID);
	glBindVertexArray(vertexArrayID);

	// Create and compile our GLSL program from the shaders
	std::cout << "Loading Shader...\n";
	programID = LoadShaders("ressources/shaders/chunkColor/chunkColor.vert",
			"ressources/shaders/chunkColor/chunkColor.frag");

	// Get a handle for our "MVP" uniform
	matrixID = glGetUniformLocation(programID, "MVP");
	lightID = glGetUniformLocation(programID, "light_dir");

	// Read our .obj file
	//std::vector<glm::vec3> vertices;
	//std::vector<glm::vec3> colors;
	//loadOBJ("ressources/models/example/cube.obj", vertices, uvs, normals);
	std::cout << "Meshing...\n";
	ChunkUtil::Mesh *mesh = ChunkUtil::greedyMesh(_chunk);

	std::cout << "Creating VAB...\n";
	// First attribute of our VAO, vertices
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, mesh->vertices.size() * sizeof(glm::vec3),
			mesh->vertices.data(), GL_STATIC_DRAW);
	verticeBufferSize = (int) mesh->vertices.size();
	std::cout << "Vertice Buffer Size: " << verticeBufferSize << "\n";

	// 2nd attribute, colors
	glGenBuffers(1, &colorBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
	glBufferData(GL_ARRAY_BUFFER, mesh->colors.size() * sizeof(glm::vec3),
			mesh->colors.data(), GL_STATIC_DRAW);

	// 3rd attribute, normals
	glGenBuffers(1, &normalBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
	glBufferData(GL_ARRAY_BUFFER, mesh->normals.size() * sizeof(glm::vec3),
			mesh->normals.data(), GL_STATIC_DRAW);

	delete mesh;
	std::cout << "Done preloading Chunk.\n";
}

void EntityChunk::draw(glm::mat4 &base, const glm::vec3& light_dir) {

	// shader program
	glUseProgram(programID);

	// uniforms
	ShaderBase::loadMVP(matrixID, base, _extraPosition, _extraRotation);
	glUniform3fv(lightID, 1, glm::value_ptr(light_dir));

	// vertex array
	glBindVertexArray(vertexArrayID);

	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glVertexAttribPointer(0,	// attribute
			3,	// size
			GL_FLOAT,	// type
			GL_FALSE,	// normalized?
			0,	// stride
			(void*) 0	// array buffer offset
			);

	// 2nd attribute buffer : colors
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
	glVertexAttribPointer(1,	// attribute
			3,	// size 3, rgb
			GL_FLOAT,	// type
			GL_FALSE,	// normalized?
			0,	// stride
			(void*) 0	// array buffer offset
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
	glDeleteProgram(programID);
	glDeleteBuffers(1, &vertexBuffer);
	glDeleteBuffers(1, &colorBuffer);
	glDeleteBuffers(1, &normalBuffer);
}
