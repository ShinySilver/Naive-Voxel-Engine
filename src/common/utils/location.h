#ifndef LOCATION_H
#define LOCATION_H

struct Location {
	glm::vec3 position, rotation, speed, rotationSpeed;

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
};

#endif
