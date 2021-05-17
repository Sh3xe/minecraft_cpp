#include "playing_state.hpp"
#include "../core/config.hpp"
#include <glad/glad.h>

PlayingState::PlayingState( Config &config ):
	m_world( config.fog_enabled ),
	// place the camera at the center of the world
	m_player(glm::vec3(CHUNK_X * WORLD_X * 0.5f, 64.0f, CHUNK_Z * WORLD_Z * 0.5f)) {
	m_player.getCamera().setSensitivity(config.sensitivity);
}

void PlayingState::update( Input &input, double delta_time ) {
	static double block_lock = 0.0; // used to limit the block breaking frequency


	m_player.update(input, m_world, delta_time);
	m_world.update(delta_time, m_player.getCamera());

	if (input.isKeyPressed(SDL_SCANCODE_Q)) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	if (input.isKeyPressed(SDL_SCANCODE_E)) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	// update block_lock
	block_lock -= delta_time;
	if(block_lock < 0.0) block_lock = 0.0;

	// BLOCK BREAKING
	if(input.getClickState().first && block_lock == 0.0) { // if the player left-click and is not locked
		// cast a ray
		glm::vec3 direction = m_player.getCamera().getDirection(),
				  position = m_player.getCamera().getPosition();

		bool done = false;
		for(int i = 0; i < 25 && !done; ++i) {
			glm::vec3 ray_pos = position + direction * static_cast<float>(i / 5.0);
			// if we find a block
			if(m_world.getBlock(ray_pos.x, ray_pos.y, ray_pos.z) != Blocks::AIR) {
				// destroy it, stop the ray cast, and reset "block_lock"
				m_world.setBlock(ray_pos.x, ray_pos.y, ray_pos.z, Blocks::AIR);
				done = true;
				block_lock = 0.25;
			}
		}
	}

	// BLOCK PLACING
	if(input.getClickState().second && block_lock == 0.0) { // if the player right-click and is not locked
		// cast a ray
		glm::vec3 direction = m_player.getCamera().getDirection(),
				  position = m_player.getCamera().getPosition();

		bool done = false;
		for(int i = 0; i < 25 && !done; ++i) {
			glm::vec3 ray_pos = position + direction * static_cast<float>(i / 5.0);
			// if we find a block
			if(m_world.getBlock(ray_pos.x, ray_pos.y, ray_pos.z) != Blocks::AIR) {
				// get the previous position
				ray_pos -= direction * 0.2f;
				// place a brick block
				m_world.setBlock(ray_pos.x, ray_pos.y, ray_pos.z, Blocks::BRICK);
				done = true;
				block_lock = 0.25;
			}
		}
	}
}

void PlayingState::render() {
	m_world.draw(m_player.getCamera());
}