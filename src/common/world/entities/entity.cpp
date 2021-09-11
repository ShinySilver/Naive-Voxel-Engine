/*
 * entity.cpp
 *
 *  Created on: 29 avr. 2020
 *      Author: silverly
 */

#include "entity.h"


Entity::Entity() :
		_location { }, _extraPosition { _location.position }, _extraRotation {
				_location.rotationSpeed } {
}

Entity::Entity(Location l) :
		_location(l), _extraPosition { l.position }, _extraRotation { l.rotation } {
}

void Entity::fastUpdate() {
	// Approximatif, mais fonctionnel
	_extraPosition += (_location.position - _extraPosition) * FIXED_RENDER_TIME
			/ FIXED_TICK_TIME;
	_extraRotation += (_location.rotation - _extraRotation) * FIXED_RENDER_TIME
			/ FIXED_TICK_TIME;
}

void Entity::fixedUpdate() {
	// Physics here
	_location.position += _location.speed * glm::vec3(FIXED_TICK_TIME); // Magic value!
	_location.rotation += _location.rotationSpeed * glm::vec3(FIXED_TICK_TIME);
}

void Entity::lock() {
	_mutex.lock();
}

void Entity::unlock() {
	_mutex.unlock();
}

const Location& Entity::getLocation() const {
	return _location;
}

const glm::vec3& Entity::getPosition() const {
	return _location.position;
}
const glm::vec3& Entity::getRotation() const {
	return _location.rotation;
}

const glm::vec3& Entity::getSpeed() const {
	return _location.speed;
}
void Entity::setSpeed(glm::vec3 &speed) {
	_location.speed = speed;
}

const glm::vec3& Entity::getRotationSpeed() const {
	return _location.rotationSpeed;
}
void Entity::setRotationSpeed(glm::vec3 &rotationSpeed) {
	_location.rotationSpeed = rotationSpeed;
}

