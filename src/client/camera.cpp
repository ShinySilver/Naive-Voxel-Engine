/*
 * EntityCamera.cpp
 *
 *  Created on: 29 avr. 2020
 *      Author: silverly
 */

#include "camera.h"
#include "../common/utils/location.h"

#include <GLFW/glfw3.h>
#include <glm/glm/vec3.hpp>
#include <glm/glm/ext/matrix_transform.hpp>

#include <cmath>
#include <mutex>

namespace camera {
    namespace {
        float _horizontalAngle = 3.14f;
        float _verticalAngle = 0.0f;
        float _speedModifier = 300.0f; // 3 units / second
        float _mouseSpeedModifier = 0.005f;
        Location _location;
        std::mutex _location_mutex;
        glm::vec3 _direction, _up, _right;
    }

    void updateControlling(GLFWwindow *window) {
        // glfwGetTime is called only once, the first time this function is called
        static double lastTime = glfwGetTime();

        // Compute time difference between current and last frame
        const double currentTime = glfwGetTime();
        const float deltaTime = float(currentTime - lastTime);

		const float movement_delta = deltaTime * _speedModifier;

        // Lock location mutex
        _location_mutex.lock();

        // Move forward
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS 
				|| glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            _location.position[0] += _direction[0] * movement_delta;
			_location.position[1] += _direction[1] * movement_delta;
        }
        // Move backward
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS 
				|| glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            _location.position[0] -= _direction[0] * movement_delta;
			_location.position[1] -= _direction[1] * movement_delta;
        }
        // Strafe _right
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS 
				|| glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            _location.position[0] += _right[0] * movement_delta;
			_location.position[1] += _right[1] * movement_delta;

        }
        // Strafe left
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS 
				|| glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            _location.position[0] -= _right[0] * movement_delta;
			_location.position[1] -= _right[1] * movement_delta;
        }
        // Custom:
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
            _location.position[2] += movement_delta;        }
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
            _location.position[2] -= movement_delta;            }

        // Lock location mutex
        _location_mutex.unlock();

        lastTime = currentTime;

    }

    void updateView(GLFWwindow *window, glm::mat4 &projectionMatrix, glm::mat4 &viewMatrix) {

        // Get mouse _location.position
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        // Reset mouse _location.position for next frame
        glfwSetCursorPos(window, 1024 / 2, 768 / 2);

        // Compute new orientation
        _horizontalAngle -= _mouseSpeedModifier * float(1024 / 2 - xpos);
        _verticalAngle -= _mouseSpeedModifier * float(768 / 2 - ypos);

        // Direction : Spherical coordinates to Cartesian coordinates conversion
        _direction = glm::vec3(cos(_verticalAngle) * sin(_horizontalAngle),
							   cos(_verticalAngle) * cos(_horizontalAngle),
                               sin(_verticalAngle));
        // _right vector
        _right = glm::vec3(sin(_horizontalAngle - 3.14f / 2.0f),
                           cos(_horizontalAngle - 3.14f / 2.0f),
						   0);

        // _up vector
        _up = glm::cross(_right, _direction);

        // Camera matrix
        viewMatrix = 
			glm::lookAt(_location.position,		// Camera is here
						_location.position +_direction,                                  
						// and looks here : at the same _location.position, plus "direction"
						_up						// Head is up (set to 0,-1,0 to look upside-down)
        );
    }

    Location get_location() {
        _location_mutex.lock();
        Location loc = _location;
        _location_mutex.unlock();
        return std::move(loc);
    }
}
