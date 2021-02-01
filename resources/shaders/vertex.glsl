#version 330 core

layout (location = 0) in vec3 a_coords;
layout (location = 1) in vec2 a_texture_coords;
layout (location = 2) in float a_light_value;

out vec2 texture_coords;
out float light_value;
out float pixel_distance;

uniform vec3 chunk_position;
uniform mat4 view;
uniform mat4 projection;

void main() {
	texture_coords = a_texture_coords;
	light_value = a_light_value;

	gl_Position = projection * view * vec4(chunk_position + a_coords.xyz, 1.0);
	pixel_distance = length( gl_Position.xyz * gl_Position.w );
}