/*
 * entity.cpp
 *
 *  Created on: 29 avr. 2020
 *      Author: silverly
 */

#include "entity.h"

#include "glm/glm/ext/matrix_transform.hpp"

// internal function

Entity::Entity() :
        _location{}, _extraPosition{_location.position}, _extraRotation{
        _location.rotationSpeed}, _is_loaded{false}, _use_count{0}, _marked_for_deletion{false} {
}

Entity::Entity(Location l) :
        _location(l), _extraPosition{l.position}, _extraRotation{l.rotation}, _is_loaded{false} {
}

void Entity::fastUpdate() {
    // Approximatif, mais fonctionnel
    _extraPosition += (_location.position - _extraPosition) * FIXED_RENDER_TIME
                      / FIXED_TICK_TIME;
    _extraRotation += (_location.rotation - _extraRotation) * FIXED_RENDER_TIME
                      / FIXED_TICK_TIME;

    _normal_matrix = glm::mat3(glm::transpose(glm::inverse(
            glm::rotate(
                    glm::rotate(
                            glm::rotate(
                                    glm::translate(glm::mat4(1.0f), _extraPosition),
                                    _extraRotation.x, glm::vec3(1.0f, 0.0f, 0.0f)),
                            _extraRotation.y, glm::vec3(0.0f, 1.0f, 0.0f)),
                    _extraRotation.z, glm::vec3(0.0f, 0.0f, 1.0f)))));
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

const Location &Entity::getLocation() const {
    return _location;
}

const glm::vec3 &Entity::getPosition() const {
    return _location.position;
}

const glm::vec3 &Entity::getRotation() const {
    return _location.rotation;
}

const glm::vec3 &Entity::getSpeed() const {
    return _location.speed;
}

void Entity::setSpeed(glm::vec3 &speed) {
    _location.speed = speed;
}

const glm::vec3 &Entity::getRotationSpeed() const {
    return _location.rotationSpeed;
}

void Entity::setRotationSpeed(glm::vec3 &rotationSpeed) {
    _location.rotationSpeed = rotationSpeed;
}

bool Entity::is_loaded() {
    return _is_loaded;
}

