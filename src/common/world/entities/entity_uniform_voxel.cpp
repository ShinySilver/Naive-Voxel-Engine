/*
 * EntityUniformVoxel.cpp
 *
 *  Created on: 30 avr. 2020
 *      Author: silverly
 */

#include "entity_uniform_voxel.h"

#include <iostream>
#include <vector>

#include "../../../../libs/glm/glm/detail/type_vec4.hpp"
#include "../../../../libs/glm/glm/ext/matrix_float4x4.hpp"
#include "../../../../libs/glm/glm/gtc/type_ptr.hpp"
#include "../../../../libs/glm/glm/vec2.hpp"
#include "../../../utils/loaders/ObjLoader.h"
#include "../../../utils/loaders/ShaderLoader.h"
#include "../../../utils/shader/ShaderBase.h"

static GLuint vertexArrayID, programID, vertexBuffer, uvBuffer, matrixID,
		colorID;
static int verticeBufferSize;

EntityUniformVoxel::EntityUniformVoxel(const glm::vec3 &position,
		const glm::vec4 &color) :
		IEntity { Location { position } } { // Wololo c'est moche
	_color = color;
}
EntityUniformVoxel::EntityUniformVoxel(const glm::vec3 &position,
		const glm::vec3 &color) :
		IEntity { Location { position } } { // Wololo c'est tjrs aussi moche
	_color = glm::vec4(color, 1.0);
}

void EntityUniformVoxel::preload() {
	std::cout << "Preloading Construct.\n";
	glGenVertexArrays(1, &vertexArrayID);
	glBindVertexArray(vertexArrayID);

	// Create and compile our GLSL program from the shaders
	if (!programID) {
		programID =
				LoadShaders(
						"ressources/shaders/uniformColor/TransformVertexShader.vertexshader",
						"ressources/shaders/uniformColor/ColorFragmentShader.fragmentshader");

		// Get a handle for our "MVP" uniform
		matrixID = glGetUniformLocation(programID, "MVP");

		// Read our .obj file
		std::vector<glm::vec3> vertices;
		std::vector<glm::vec2> uvs;
		std::vector<glm::vec3> normals; // Won't be used at the moment.
		loadOBJ("ressources/models/example/cube.obj", vertices, uvs,
				normals);

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

		// Get a handle for our "myTextureSampler" uniform
		colorID = glGetUniformLocation(programID, "fragmentColor");
	}

	std::cout << "Done preloading Construct.\n";
}

void EntityUniformVoxel::draw(glm::mat4 &base) {
	// Use our shader
	glUseProgram(programID);

	ShaderBase::loadMVP(matrixID, base, _extraPosition, _extraRotation);
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

	// Set our "myTextureSampler" sampler to user Texture Unit 0
	glUniform4fv(colorID, 1, glm::value_ptr(_color));

	// Draw the triangle !
	glDrawArrays(GL_TRIANGLES, 0, verticeBufferSize);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glBindVertexArray(0);
}

void EntityUniformVoxel::unload() {
	glDeleteVertexArrays(1, &vertexArrayID);
	glDeleteProgram(programID);
	glDeleteBuffers(1, &vertexBuffer);
	glDeleteBuffers(1, &uvBuffer);
	glDeleteBuffers(1, &colorID); // BWAAAAK
}
