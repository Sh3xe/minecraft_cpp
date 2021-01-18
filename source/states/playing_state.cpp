#include "playing_state.hpp"
#include <glad/glad.h>

PlayingState::PlayingState():
	m_shader("resources/shaders/vertex.glsl", "resources/shaders/fragment.glsl") {

	glGenBuffers(1, &m_vbo);
	glGenVertexArrays(1, &m_vao);

	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

	float vertices[] = {
		 0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,
		 0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f,
	};

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void PlayingState::update( Input &input, double delta_time ) {
}

void PlayingState::render() {
	m_shader.use();
	glBindVertexArray(m_vao);
	glDrawArrays(GL_TRIANGLES, 0, 3);
}