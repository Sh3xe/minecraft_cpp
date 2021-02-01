#version 330 core

in vec2 texture_coords;
in float light_value;
in float pixel_distance;

uniform sampler2D u_texture;

const vec4 fog_color = vec4(0.0, 0.6, 1.0, 1.0);

void main() {
	vec4 texture_color = texture(u_texture, texture_coords / 16) * light_value;
	
	float fog = pixel_distance / 5000;

	if(fog > 1)
		gl_FragColor = fog_color;
	else
		gl_FragColor = mix(texture_color, fog_color, fog);
}