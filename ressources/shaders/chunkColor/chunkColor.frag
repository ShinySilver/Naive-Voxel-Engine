#version 450 core

// inputs
in vec3 frag_color;
in vec3 frag_normal;

// outputs
out vec3 color;

// uniforms
uniform vec3 light_dir;

void main() {

	//temporary
	vec3 light_color = vec3(1.0, 1.0, 0.8);

	float ambient_factor = 0.1;
	float ambient = ambient_factor;

	float diffuse_factor = 0.5;
	//TODO add matrix to transform normals into world coords
	float diffuse = diffuse_factor * max(dot(frag_normal, light_dir), 0.0);

	color = frag_color * (ambient + diffuse) * light_color;
}
