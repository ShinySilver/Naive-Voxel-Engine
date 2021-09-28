#version 450 core

// inputs
in vec3 frag_color;
in vec3 frag_normal;
in vec3 light_vector;

// outputs
out vec3 color;

void main() {

	//temporary
	vec3 light_color = vec3(1.0, 1.0, 0.8);

	float ambient_factor = 0.1;
	float ambient = ambient_factor;

	float diffuse_factor = 0.5;
	float diffuse  = diffuse_factor * max(dot(frag_normal, light_vector), 0.0);

	color = frag_color * (ambient + diffuse) * light_color;
}
