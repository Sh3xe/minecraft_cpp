#include "playing_state.hpp"

#include <glad/glad.h>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../opengl_wrapper/gldebug.hpp"

#define NDEBUG

PlayingState::PlayingState():
	m_shader("resources/shaders/vertex.glsl", "resources/shaders/fragment.glsl"),
	m_texture("resources/images/dev.png"),
	m_camera(glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f)) {

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
	m_camera.update(input, delta_time);
}

void PlayingState::render() {
	glm::mat4 model_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(-1.f, 0.f, 0.f));
	glm::mat4 view_matrix = m_camera.getViewMatrix();
	glm::mat4 projection_matrix = glm::perspective(3.14159f/2.0f, 16.0f/9.0f, 0.01f, 100.0f);

	logGlErrors();

	m_shader.use();
	glBindVertexArray(m_vao);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_texture.getId());
	m_shader.setInt("u_texture", 0);

	m_shader.setMat4("model", glm::value_ptr( model_matrix ));
	m_shader.setMat4("view", glm::value_ptr( view_matrix ));
	m_shader.setMat4("projection", glm::value_ptr( projection_matrix ));

	glDrawArrays(GL_TRIANGLES, 0, 6);

}