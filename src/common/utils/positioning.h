#ifndef POSITIONING_H
#define POSITIONING_H

typedef glm::vec3 ChunkPos;
typedef glm::vec3 WorldPos;

typedef struct Location {
	WorldPos position;
    glm::vec3 rotation, speed, rotationSpeed;

	Location(glm::vec3 pos, glm::vec3 spd, glm::vec3 rot, glm::vec3 rotSpd) {
		position = pos;
		rotation = rot;
		speed = spd;
		rotationSpeed = rotSpd;
	}
	Location(glm::vec3 pos, glm::vec3 spd) {
		position = pos;
		rotation = glm::vec3{0.0f};
		speed = spd;
		rotationSpeed = glm::vec3{0.0f};
	}
	Location(glm::vec3 pos) {
		position = pos;
		rotation = glm::vec3{0.0f};
		speed = glm::vec3{0.0f};
		rotationSpeed = glm::vec3{0.0f};
	}
	Location() {
		position = glm::vec3{0.0f};
		rotation = glm::vec3{0.0f};
		speed = glm::vec3{0.0f};
		rotationSpeed = glm::vec3{0.0f};
	}
} Location;

#include "positioning.tcc"

#endif
