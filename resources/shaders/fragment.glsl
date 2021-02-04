#version 330 core

in vec2 texture_coords;
in float light_value;
in float pixel_distance;

uniform sampler2D u_texture;
uniform int fog_enabled;

const vec4 fog_color = vec4(0.0, 0.6, 1.0, 1.0);
const int fog_distance = 2000;

void main() {
	vec4 texture_color = texture(u_texture, texture_coords / 16) * light_value;
	
	// pseudo fog implementation
	float fog = 0.0;
	if(pixel_distance > fog_distance && fog_enabled == 1) {
		fog = (pixel_distance - fog_distance) / 500;
	}

	if(fog > 1)
		gl_FragColor = fog_color;
	else
		gl_FragColor = mix(texture_color, fog_color, fog);
}