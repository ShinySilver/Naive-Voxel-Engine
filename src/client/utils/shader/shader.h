/*
 * IShader.h
 *
 *  Created on: 4 mai 2020
 *      Author: silverly
 */

#ifndef UTILS_SHADER_ISHADER_H_
#define UTILS_SHADER_ISHADER_H_

class Shader {
public:
	IShader() = default;
	virtual ~IShader() = default;

	virtual void preload() = 0;
	virtual void draw() = 0;
	virtual void unload() = 0;
};

#endif /* UTILS_SHADER_ISHADER_H_ */
