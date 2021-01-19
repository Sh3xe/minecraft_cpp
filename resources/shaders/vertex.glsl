#version 330 core

layout (location = 0) in vec3 a_position;
layout (location = 1) in vec2 a_texture_coords;

out vec2 texture_coords;

void main() {
    texture_coords = a_texture_coords;
    gl_Position = vec4(a_position.xyz, 1.0);
}