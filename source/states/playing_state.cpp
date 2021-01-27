#include "playing_state.hpp"
#include <glad/glad.h>
//#include <glad/glad.h>

PlayingState::PlayingState():
	m_camera(glm::vec3(1.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f)) {
}

void PlayingState::update( Input &input, double delta_time ) {
	m_camera.update(input, delta_time);
	//m_world.update(delta_time);

	if (input.isKeyPressed(SDL_SCANCODE_Q)) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	if (input.isKeyPressed(SDL_SCANCODE_E)) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
}

void PlayingState::render() {
	m_world.draw(m_camera);
}