#version 330 core

layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_color;

out vec4 color;

void main() {
    color = vec4(a_color.xyz, 1.0);
    gl_Position = vec4(a_position, 1.0);
}