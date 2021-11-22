#version 450 core

// inputs
layout (location = 0) in vec3 vert_position;
layout (location = 1) in vec3 vert_color;
layout (location = 2) in vec3 vert_normal;

// outputs
out vec3 frag_color;
out vec3 frag_normal;
out vec3 frag_position;
out vec3 light_dir;

// uniforms
uniform mat4 MVP;
uniform mat3 normal_mat;
uniform vec3 light_position;

void main() {	

	// computing vertex position
	gl_Position =  MVP * vec4(vert_position, 1);

	// computing data for fragment shader
	frag_color = vert_color;
	frag_normal = normal_mat * vert_normal;
	frag_position = vert_position;
	light_dir = normalize(vert_position - light_position);
}

