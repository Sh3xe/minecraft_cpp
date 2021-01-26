#version 330 core

layout (location = 0) in vec3 a_coords;
layout (location = 1) in vec2 a_texture_coords;
layout (location = 2) in float a_light_value;

out vec2 texture_coords;
out float light_value;

uniform mat4 view;
//uniform mat4 model;
uniform mat4 projection;

void main() {
	texture_coords = a_texture_coords;
	light_value = a_light_value;

	gl_Position = projection * view /** model*/ * vec4(a_coords.xyz, 1.0);
}