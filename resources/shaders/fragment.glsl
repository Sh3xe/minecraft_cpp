#version 330 core

in vec2 texture_coords;
in float light_value;

uniform sampler2D u_texture;

void main() {
    gl_FragColor = texture(u_texture, texture_coords * 16) * light_value;
    
    //gl_FragColor = vec4(0.0, 1.0, 0.2, 1.0);
}