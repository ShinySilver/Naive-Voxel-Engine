/*
 * EntityCamera.h
 *
 *  Created on: 29 avr. 2020
 *      Author: silverly
 */

#ifndef WORLD_ENTITY_ENTITYCAMERA_H_
#define WORLD_ENTITY_ENTITYCAMERA_H_

#include <GLFW/glfw3.h>
#include <glm/glm/ext/matrix_float4x4.hpp>
#include "../common/utils/location.h"

namespace camera {
    void updateControlling(GLFWwindow *);

    void updateView(GLFWwindow *, glm::mat4 &, glm::mat4 &);

    Location get_location();

    void fixedUpdate();
};

#endif /* WORLD_ENTITY_ENTITYCAMERA_H_ */
