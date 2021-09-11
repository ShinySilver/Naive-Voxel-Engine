/*
 * Shader.h
 *
 *  Created on: 3 mai 2020
 *      Author: silverly
 */

#ifndef UTILS_SHADER_SHADERBASE_H_
#define UTILS_SHADER_SHADERBASE_H_

#include <GL/glew.h>

namespace ShaderBase {

inline void loadMVP(GLuint matrixID, glm::mat4 &base, glm::vec3 &position,
		glm::vec3 &rotation) {
	glUniformMatrix4fv(matrixID, 1, GL_FALSE,
			glm::value_ptr(
					glm::rotate(
							glm::rotate(
									glm::rotate(glm::translate(base, position),
											rotation.x,
											glm::vec3(1.0f, 0.0f, 0.0f)),
									rotation.y, glm::vec3(0.0f, 1.0f, 0.0f)),
							rotation.z, glm::vec3(0.0f, 0.0f, 1.0f))));
}

}

#endif /* UTILS_SHADER_SHADERBASE_H_ */
