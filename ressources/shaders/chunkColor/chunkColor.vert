#version 450 core

// inputs
layout (location = 0) in vec3 vertexPosition_modelspace;
layout (location = 1) in vec3 vertexColor;
layout (location = 2) in vec3 vert_normal;

// outputs
out vec3 frag_color;
out vec3 frag_normal;

// uniforms
uniform mat4 MVP;

void main() {	

	// computing vertex position
	gl_Position =  MVP * vec4(vertexPosition_modelspace,1);

	// computing data for fragment shader
	frag_color = vertexColor;
	frag_normal = vert_normal;
}

