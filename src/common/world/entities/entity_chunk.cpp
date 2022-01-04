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

GLuint EntityChunk::programID = 0;
GLuint EntityChunk::matrixID = 0;

EntityChunk::EntityChunk(Chunk chunk) :
        _chunk(std::move(chunk)) {}

EntityChunk::EntityChunk(Chunk chunk, Location loc) :
        Entity(loc), _chunk(std::move(chunk)) {}

void EntityChunk::preload() {
    std::cout << "Preloading Chunk.\n";

    // Read our .obj file
    //std::vector<glm::vec3> vertices;
    //std::vector<glm::vec3> colors;
    //loadOBJ("ressources/models/example/cube.obj", vertices, uvs, normals);
    Location loc = getLocation();
    std::cout << "Meshing chunk at" << loc.position[0] << "; "
              << loc.position[1] << "; "
              << loc.position[2] << "...\n";
    _mesh = ChunkUtil::greedyMesh(_chunk);
}

void EntityChunk::load() {
    std::cout << "Loading Chunk.\n";

    // Our VAO
    glGenVertexArrays(1, &vertexArrayID);
    glBindVertexArray(vertexArrayID);

    if (!programID){
        // The 1st time, create and compile our GLSL program from the shaders
        std::cout << "Loading Shader...\n";
        programID = LoadShaders("ressources/shaders/chunkColor/chunkColor.vs",
                                "ressources/shaders/chunkColor/chunkColor.fs");

        // Get a handle for our "MVP" uniform
        matrixID = glGetUniformLocation(programID, "MVP");
    }

    std::cout << "Creating VAB...\n";
    // First attribute of our VAO, vertices
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, _mesh->vertices.size() * sizeof(glm::vec3),
                 _mesh->vertices.data(), GL_STATIC_DRAW);
    verticeBufferSize = (int) _mesh->vertices.size();
    std::cout << "Vertice Buffer Size: " << verticeBufferSize << "\n";

    // 2nd attribute, colors
    glGenBuffers(1, &colorBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
    glBufferData(GL_ARRAY_BUFFER, _mesh->colors.size() * sizeof(glm::vec3),
                 _mesh->colors.data(),
                 GL_STATIC_DRAW);

    delete _mesh;
    std::cout << "Done preloading Chunk.\n";
}

void EntityChunk::draw(glm::mat4 &base) {
    // Use our shader
    glUseProgram(programID);

    ShaderBase::loadMVP(matrixID, base, _extraPosition, _extraRotation);
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

    // Draw the triangles !
    glDrawArrays(GL_TRIANGLES, 0, verticeBufferSize);

    // And then we unbind everything
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glBindVertexArray(0);
}

void EntityChunk::unload() {
    glDeleteVertexArrays(1, &vertexArrayID);
    glDeleteProgram(programID);
    glDeleteBuffers(1, &vertexBuffer);
    glDeleteBuffers(1, &colorBuffer);
}
