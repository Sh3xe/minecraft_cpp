#include "playing_state.hpp"
#include <glad/glad.h>

PlayingState::PlayingState():
	m_camera(glm::vec3(32.0f, 48.0f, 32.0f), glm::vec3(0.f, 1.f, 0.f)) {
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

	if(input.getClickState().first) { // if the player left-click
		// cast a ray
		glm::vec3 direction = m_camera.getDirection(),
				  position = m_camera.getPosition();

		bool done = false;
		for(int i = 0; i < 10 && !done; ++i) {
			glm::vec3 ray_pos = position + direction * static_cast<float>(i / 2);
			if(m_world.getBlock(ray_pos.x, ray_pos.y, ray_pos.z) != Blocks::AIR) {
				m_world.setBlock(ray_pos.x, ray_pos.y, ray_pos.z, Blocks::AIR);
				done = true;
			}
		}
	}
}

void PlayingState::render() {
	m_world.draw(m_camera);
}