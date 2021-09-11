/*
 * EntityVoxel.cpp
 *
 *  Created on: 30 avr. 2020
 *      Author: silverly
 */

#include "entity_voxel.h"

#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>
#include <iostream>
#include <vector>

#include "../../../utils/loaders/ObjLoader.h"
#include "../../../utils/loaders/ShaderLoader.h"
#include "../../../utils/loaders/TextureLoader.h"
#include "../../../utils/shader/ShaderBase.h"

void EntityVoxel::preload() {
	std::cout << "Preloading voxel.\n";
	glGenVertexArrays(1, &vertexArrayID);
	glBindVertexArray(vertexArrayID);

	// Create and compile our GLSL program from the shaders
	programID = LoadShaders(
			"ressources/shaders/texture/TransformVertexShader.vertexshader",
			"ressources/shaders/texture/TextureFragmentShader.fragmentshader");

	// Get a handle for our "MVP" uniform
	matrixID = glGetUniformLocation(programID, "MVP");

	// Load the texture
	texture = loadDDS("ressources/models/example/uvmap.DDS");

	// Get a handle for our "myTextureSampler" uniform
	textureID = glGetUniformLocation(programID, "myTextureSampler");

	// Read our .obj file
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals; // Won't be used at the moment.
	loadOBJ("ressources/models/example/cube.obj", vertices, uvs, normals);

	// Load it into a VBO
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3),
			&vertices[0], GL_STATIC_DRAW);
	verticeBufferSize = (int) vertices.size();

	glGenBuffers(1, &uvBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
	glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0],
	GL_STATIC_DRAW);

	std::cout << "Done preloading voxel.\n";
}

void EntityVoxel::draw(glm::mat4 &base) {
	// Use our shader
	glUseProgram(programID);

	ShaderBase::loadMVP(matrixID, base, _extraPosition, _extraRotation);
	glBindVertexArray(vertexArrayID);

	// Bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	// Set our "myTextureSampler" sampler to user Texture Unit 0
	glUniform1i(textureID, 0);

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

	// 2nd attribute buffer : UVs
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
	glVertexAttribPointer(1,	// attribute
			2,	// size
			GL_FLOAT,	// type
			GL_FALSE,	// normalized?
			0,	// stride
			(void*) 0	// array buffer offset
			);

	// Draw the triangle !
	glDrawArrays(GL_TRIANGLES, 0, verticeBufferSize);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glBindVertexArray(0);
}

void EntityVoxel::unload() {
	glDeleteVertexArrays(1, &vertexArrayID);
	glDeleteProgram(programID);
	glDeleteBuffers(1, &vertexBuffer);
	glDeleteBuffers(1, &uvBuffer);
	glDeleteTextures(1, &textureID);
}
