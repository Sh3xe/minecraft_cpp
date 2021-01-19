#version 330 core

out vec4 o_color;

in vec2 texture_coords;
uniform sampler2D u_texture;

void main() {
    o_color = texture(u_texture, texture_coords);
    //o_color = vec4(1.0, 0.0, 0.0, 1.0);
}