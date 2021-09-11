/*
 * IEntity.h
 *
 *  Created on: 29 avr. 2020
 *      Author: silverly
 */

#ifndef WORLD_ENTITY_IENTITY_H_
#define WORLD_ENTITY_IENTITY_H_

#include <mutex>

#include "../../../libs/glm/glm/ext/matrix_float4x4.hpp"
#include "../../../libs/glm/glm/vec3.hpp"
#include "../../utils/Location.h"

#define FIXED_TICK_TIME (1/20.0f)
#define FIXED_RENDER_TIME (1/60.0f)

class IEntity {
public:
	IEntity();
	IEntity(Location);
	virtual ~IEntity() = default;

	/**
	 * Those are specific to each entity
	 */
	virtual void preload() = 0;
	virtual void draw(glm::mat4&) = 0;
	virtual void unload() = 0;

	/**
	 * Both the render thread and the update thread will iterate over entities. However, there should not be a race over
	 * them. At last, those functions should only be called by the API.
	 */
	void lock();
	void unlock();

	/**
	 * Called by the world thread 20 time per sec. To be overriden for sentient entities.
	 */
	virtual void fixedUpdate();
	virtual void fastUpdate();

	/**
	 * Location is updated every fixed update, and extrapolated using speed during render. No teleportation from outside.
	 */
	const Location& getLocation() const;

	const glm::vec3& getPosition() const;
	const glm::vec3& getRotation() const;

	const glm::vec3& getSpeed() const;
	void setSpeed(glm::vec3 &speed);

	const glm::vec3& getRotationSpeed() const;
	void setRotationSpeed(glm::vec3 &rotationSpeed);

protected:
	Location _location;
	glm::vec3 _extraPosition, _extraRotation;
private:
	std::mutex _mutex;

};

#endif /* WORLD_ENTITY_IENTITY_H_ */
