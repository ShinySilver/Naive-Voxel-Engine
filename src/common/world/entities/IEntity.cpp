/*
 * IEntity.cpp
 *
 *  Created on: 29 avr. 2020
 *      Author: silverly
 */

#include "IEntity.h"

#include "../../../libs/glm/glm/detail/type_vec3.hpp"
#include "../../../libs/glm/glm/ext/matrix_transform.hpp"
#include "../../../libs/glm/glm/ext/quaternion_transform.hpp"
#include "../../../libs/glm/glm/gtc/type_ptr.hpp"
#include "../../client/Camera.h"
#include "../../client/Client.h"

IEntity::IEntity() :
		_location { }, _extraPosition { _location.position }, _extraRotation {
				_location.rotationSpeed } {
}

IEntity::IEntity(Location l) :
		_location(l), _extraPosition { l.position }, _extraRotation { l.rotation } {
}

void IEntity::fastUpdate() {
	// Approximatif, mais fonctionnel
	_extraPosition += (_location.position - _extraPosition) * FIXED_RENDER_TIME
			/ FIXED_TICK_TIME;
	_extraRotation += (_location.rotation - _extraRotation) * FIXED_RENDER_TIME
			/ FIXED_TICK_TIME;
}

void IEntity::fixedUpdate() {
	// Physics here
	_location.position += _location.speed * glm::vec3(FIXED_TICK_TIME); // Magic value!
	_location.rotation += _location.rotationSpeed * glm::vec3(FIXED_TICK_TIME);
}

void IEntity::lock() {
	_mutex.lock();
}

void IEntity::unlock() {
	_mutex.unlock();
}

const Location& IEntity::getLocation() const {
	return _location;
}

const glm::vec3& IEntity::getPosition() const {
	return _location.position;
}
const glm::vec3& IEntity::getRotation() const {
	return _location.rotation;
}

const glm::vec3& IEntity::getSpeed() const {
	return _location.speed;
}
void IEntity::setSpeed(glm::vec3 &speed) {
	_location.speed = speed;
}

const glm::vec3& IEntity::getRotationSpeed() const {
	return _location.rotationSpeed;
}
void IEntity::setRotationSpeed(glm::vec3 &rotationSpeed) {
	_location.rotationSpeed = rotationSpeed;
}

