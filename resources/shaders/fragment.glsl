#version 330 core

in vec2 texture_coords;
uniform Sampler2D u_texture;

out vec4 o_color;

void main() {
    o_color = texture(u_texture, texture_coords);
}