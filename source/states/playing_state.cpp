#include "playing_state.hpp"
#include "../core/config.hpp"
#include "gl_functions.hpp"
#include "core/logger.hpp"

PlayingState::PlayingState( Config &config ):
	m_world(),
	// place the camera at the center of the world
	m_player(glm::vec3(0.0f, 64.0f, 0.0f))
{
	m_player.get_camera().set_sensitivity(config.sensitivity);
	
	if( !BlockDB::get().load_from_file("resources/blocks/blocks.json"))
		SD_ERROR("Impossible de charger les informations relatives aux blocs");
}

void PlayingState::update( Input &input, double delta_time )
{
	static double block_lock = 0.0; // used to limit the block breaking frequency


	m_player.update(input, m_world, delta_time);
	m_world.update(delta_time, m_player.get_camera());

	if (input.get_key(SDL_SCANCODE_Q)) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	if (input.get_key(SDL_SCANCODE_E)) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	// update block_lock
	block_lock -= delta_time;
	if(block_lock < 0.0) block_lock = 0.0;

	// BLOCK BREAKING
	if(input.get_mouse_state().first && block_lock == 0.0)
	{   // if the player left-click and is not locked
		// cast a ray
		glm::vec3 direction = m_player.get_camera().get_direction(),
				  position = m_player.get_camera().get_position();

		bool done = false;
		for(int i = 0; i < 25 && !done; ++i) {
			glm::vec3 ray_pos = position + direction * static_cast<float>(i / 5.0);
			// if we find a block
			if(m_world.get_block(ray_pos.x, ray_pos.y, ray_pos.z))
			{
				// destroy it, stop the ray cast, and reset "block_lock"
				m_world.set_block(ray_pos.x, ray_pos.y, ray_pos.z, 0); //__TODO
				done = true;
				block_lock = 0.25;
			}
		}
	}

	// BLOCK PLACING
	if(input.get_mouse_state().second && block_lock == 0.0) // if the player right-click and is not locked
	{
		// cast a ray
		glm::vec3 direction = m_player.get_camera().get_direction(),
				  position = m_player.get_camera().get_position();

		bool done = false;
		for(int i = 0; i < 25 && !done; ++i)
		{
			glm::vec3 ray_pos = position + direction * static_cast<float>(i / 5.0);
			// if we find a block
			if( m_world.get_block(ray_pos.x, ray_pos.y, ray_pos.z) != 0 ) //__TODO
			{
				// get the previous position
				ray_pos -= direction * 0.2f;
				// place a brick block
				m_world.set_block(ray_pos.x, ray_pos.y, ray_pos.z, 3); //__TODO
				done = true;
				block_lock = 0.25;
			}
		}
	}
}

void PlayingState::render()
{
	m_world.draw(m_player.get_camera());
}