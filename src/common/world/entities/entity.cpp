/*
 * entity.cpp
 *
 *  Created on: 29 avr. 2020
 *      Author: silverly
 */

#include "entity.h"

#include "../../../libs/glm/glm/detail/type_vec3.hpp"
#include "../../../libs/glm/glm/ext/matrix_transform.hpp"
#include "../../../libs/glm/glm/ext/quaternion_transform.hpp"
#include "../../../libs/glm/glm/gtc/type_ptr.hpp"
#include "../../client/Camera.h"
#include "../../client/Client.h"

entity::entity() :
		_location { }, _extraPosition { _location.position }, _extraRotation {
				_location.rotationSpeed } {
}

entity::entity(Location l) :
		_location(l), _extraPosition { l.position }, _extraRotation { l.rotation } {
}

void entity::fastUpdate() {
	// Approximatif, mais fonctionnel
	_extraPosition += (_location.position - _extraPosition) * FIXED_RENDER_TIME
			/ FIXED_TICK_TIME;
	_extraRotation += (_location.rotation - _extraRotation) * FIXED_RENDER_TIME
			/ FIXED_TICK_TIME;
}

void entity::fixedUpdate() {
	// Physics here
	_location.position += _location.speed * glm::vec3(FIXED_TICK_TIME); // Magic value!
	_location.rotation += _location.rotationSpeed * glm::vec3(FIXED_TICK_TIME);
}

void entity::lock() {
	_mutex.lock();
}

void entity::unlock() {
	_mutex.unlock();
}

const Location& entity::getLocation() const {
	return _location;
}

const glm::vec3& entity::getPosition() const {
	return _location.position;
}
const glm::vec3& entity::getRotation() const {
	return _location.rotation;
}

const glm::vec3& entity::getSpeed() const {
	return _location.speed;
}
void entity::setSpeed(glm::vec3 &speed) {
	_location.speed = speed;
}

const glm::vec3& entity::getRotationSpeed() const {
	return _location.rotationSpeed;
}
void entity::setRotationSpeed(glm::vec3 &rotationSpeed) {
	_location.rotationSpeed = rotationSpeed;
}

