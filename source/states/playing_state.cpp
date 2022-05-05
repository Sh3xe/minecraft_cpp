#include "playing_state.hpp"
#include "../core/config.hpp"
#include "gl_functions.hpp"
#include "core/logger.hpp"

PlayingState::PlayingState( Config &config ):
	m_world( config ),
	m_player(glm::vec3(0.0f, 200.0f, 0.0f))
{
	m_player.get_camera().set_sensitivity(config.sensitivity);
}

void PlayingState::update( Input &input, double delta_time )
{
	static double block_lock = 0.0; // used to limit the block breaking frequency


	m_player.update(input, m_world, delta_time);
	m_world.update(delta_time, m_player.get_camera());

	if (input.get_key(SDL_SCANCODE_Q))
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	if (input.get_key(SDL_SCANCODE_E))
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


	// update block_lock
	block_lock -= delta_time;
	if(block_lock < 0.0) block_lock = 0.0;

	// BLOCK BREAKING
	if(input.get_mouse_state().first && block_lock == 0.0)
	{   // si le joueur clique 
		glm::vec3 direction = m_player.get_camera().get_direction(),
				  position = m_player.get_camera().get_position();

		bool done = false;
		for(int i = 0; i < 30 && !done; ++i)
		{
			glm::vec3 ray_pos = position + direction * static_cast<float>(i / 4.0);
			int rx = floor( ray_pos.x );
			int ry = floor( ray_pos.y );
			int rz = floor( ray_pos.z );

			// on trouve le block sur lequel il click
			if(m_world.get_block(rx, ry, rz))
			{
				// on le supprimer et réinitialise le compteur
				m_world.set_block(rx, ry, rz, 0);
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
		for(int i = 0; i < 30 && !done; ++i)
		{
			glm::vec3 ray_pos = position + direction * static_cast<float>(i / 4.0);
			int rx = floor( ray_pos.x );
			int ry = floor( ray_pos.y );
			int rz = floor( ray_pos.z );
			// si on trouve un block
			if( m_world.get_block(rx, ry, rz) != 0 )
			{
				// on récupère la dernière position
				ray_pos -= direction * 0.2f;
				rx = floor( ray_pos.x );
				ry = floor( ray_pos.y );
				rz = floor( ray_pos.z );

				// on place un bloc
				m_world.set_block(rx, ry, rz, 3);
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