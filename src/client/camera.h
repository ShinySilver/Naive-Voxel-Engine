/*
 * EntityCamera.h
 *
 *  Created on: 29 avr. 2020
 *      Author: silverly
 */

#ifndef IVY_CAMERA_H_
#define IVY_CAMERA_H_

#include <GLFW/glfw3.h>
#include <glm/glm/ext/matrix_float4x4.hpp>
#include "../common/utils/positioning.h"

namespace camera {
    void updateControlling(GLFWwindow *);

    void updateView(GLFWwindow *, glm::mat4 &, glm::mat4 &);

    Location get_location();
};

#endif /* IVY_CAMERA_H_ */
