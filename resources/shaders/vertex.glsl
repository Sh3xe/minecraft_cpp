#version 330 core

layout (location = 0) in int a_data;

out vec2 texture_coords;
out float light_value;

uniform vec3 chunk_position;
uniform mat4 view;
uniform mat4 projection;

const float light_values[8] = float[8](
	0.9,
	0.7,
	1.0,
	0.5,
	0.9,
	0.7,
	1.0,
	1.0
);

void main() {
	texture_coords = vec2((a_data >> 8) & 0x1F, (a_data >> 3) & 0x1F);
	light_value = light_values[ a_data & 0x07 ];
	vec3 coords = vec3((a_data >> 27) & 0x1F, (a_data >> 13) & 0x1FF, (a_data >> 22) & 0x1F);

	gl_Position = projection * view * vec4(chunk_position + coords.xyz, 1.0);
}