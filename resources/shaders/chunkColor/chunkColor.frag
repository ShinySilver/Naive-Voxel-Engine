#version 450 core

// inputs
in vec3 frag_color;
in vec3 frag_normal;
in vec3 frag_position;
in vec3 light_dir;

// outputs
out vec4 color;

// uniforms
uniform vec3 view_position;

void main() {

	//temporary
	vec3 light_color = vec3(1.0, 1.0, 0.8);

	float ambient_factor = 0.3;
	float ambient = ambient_factor;

	float diffuse_factor = 0.6;
	float diffuse = diffuse_factor * max(dot(frag_normal, light_dir), 0.0);

	float specular_factor = 0.4;
	vec3 view_dir = normalize(view_position - frag_position);
	vec3 reflect_dir = reflect(-light_dir, frag_normal);
	float specular = specular_factor * pow(max(dot(view_dir, reflect_dir), 0.0), 32);

	color = vec4(frag_color * (ambient + diffuse + specular) * light_color, 1.0);
}

