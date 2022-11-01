#include "playing_state.hpp"
#include "core/settings.hpp"
#include <glad/glad.h>
#include "core/logger.hpp"
#include "graphics/renderer.hpp"

PlayingState::PlayingState( Settings &settings ):
	m_world( settings ),
	m_player(glm::vec3(0.0f, 200.0f, 0.0f))
{
	m_player.get_camera().sensitivity = settings.sensitivity;
	Input::get().add_click_callback(
		[this]( MouseButton b ) -> void
		{
			this->on_click( b );
		}, "breakblocks"
	);
}

PlayingState::~PlayingState()
{
	Input::get().remove_click_callback("breakblocks");
}

void PlayingState::update( double delta_time )
{
	m_player.update(m_world, delta_time);
	m_world.update(delta_time, m_player.get_camera());

	// TODO: ajouter un mode de rendu DEBUG ou un layer debug ou foutre ça dans le Renderer...
	if (Input::get().get_key(SDL_SCANCODE_Q))
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	if (Input::get().get_key(SDL_SCANCODE_E))
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

}

void PlayingState::render()
{

}

void PlayingState::on_click( MouseButton b )
{
	// Destruction de blocs
	if( b == MouseButton::left )
	{   // si le joueur clique 
		glm::vec3 direction = m_player.get_camera().direction,
				  position = m_player.get_camera().position;

		bool done = false;
		for(int i = 0; i < 30 && !done; ++i)
		{
			glm::vec3 ray_pos = position + direction * static_cast<float>(i / 7.0);
			int rx = floor( ray_pos.x );
			int ry = floor( ray_pos.y );
			int rz = floor( ray_pos.z );

			// on trouve le block sur lequel il click
			if( m_world.get_block(rx, ry, rz) != blk::BlockType::air )
			{
				// on le supprimer et réinitialise le compteur
				m_world.set_block(rx, ry, rz, blk::BlockType::air );
				done = true;
			}
		}
	}

	// placement blocs
	if( b == MouseButton::right ) // if the player right-click and is not locked
	{
		// cast a ray
		glm::vec3 direction = m_player.get_camera().direction,
				  position = m_player.get_camera().position;

		bool done = false;
		for(int i = 0; i < 30 && !done; ++i)
		{
			glm::vec3 ray_pos = position + direction * static_cast<float>(i / 7.0);
			int rx = floor( ray_pos.x );
			int ry = floor( ray_pos.y );
			int rz = floor( ray_pos.z );
			// si on trouve un block
			if( m_world.get_block(rx, ry, rz) != blk::BlockType::air )
			{
				// on récupère la dernière position
				ray_pos -= direction * 0.2f;
				rx = floor( ray_pos.x );
				ry = floor( ray_pos.y );
				rz = floor( ray_pos.z );

				// on place un bloc
				m_world.set_block(rx, ry, rz, blk::BlockType::brick );
				done = true;
			}
		}
	}
}
