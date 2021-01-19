#include "playing_state.hpp"
#include <glad/glad.h>
#include <iostream>

#include "../opengl_wrapper/gldebug.hpp"

#define NDEBUG

PlayingState::PlayingState():
	m_shader("resources/shaders/vertex.glsl", "resources/shaders/fragment.glsl"),
	m_texture("resources/images/dev.png") {

	glGenBuffers(1, &m_vbo);
	glGenVertexArrays(1, &m_vao);

	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

	float vertices[] = {
		-0.5f, -0.5f, 0.0f,  0.0f, 1.0f,
		 0.5f,  0.5f, 0.0f,  1.0f, 0.0f,
		-0.5f,  0.5f, 0.0f,  0.0f, 0.0f,
		-0.5f, -0.5f, 0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f, 0.0f,  1.0f, 1.0f,
		 0.5f,  0.5f, 0.0f,  1.0f, 0.0f,
	};

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	if(!m_texture || !m_shader)
		std::cout << "error\n";


}

void PlayingState::update( Input &input, double delta_time ) {
}

void PlayingState::render() {
	
	logGlErrors();

	m_shader.use();
	glBindVertexArray(m_vao);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_texture.getId());
	m_shader.setInt("u_texture", 0);

	glDrawArrays(GL_TRIANGLES, 0, 6);

}