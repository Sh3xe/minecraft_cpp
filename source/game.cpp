#include "game.hpp"

#include <chrono>
#include <thread>
#include <SDL_video.h>

#include "gl_functions.hpp"
#include "states/playing_state.hpp"

using namespace std::chrono;
using dseconds = duration<double>;

Game::Game( const Config &config, SDL_Window *window ):
	m_config(config),
	m_window( window )
{
	m_states.push_back( std::unique_ptr<PlayingState>( new PlayingState(m_config) ) );
}

Game::~Game()
{
}

void Game::handle_events()
{
	// handles keyboard events
	SDL_Event event;
	while(SDL_PollEvent(&event))
	{
		if(event.type == SDL_QUIT)
			m_should_close = true;
		if(event.type == SDL_KEYDOWN)
			m_input.handle_keydown(event.key.keysym.scancode);
		if(event.type == SDL_KEYUP)
			m_input.handle_keyup(event.key.keysym.scancode);
		if(event.type == SDL_MOUSEMOTION)
			m_input.add_mouse_pos( event.motion.xrel, event.motion.yrel );
	}

	// MOUSE MOVEMENT CALCULATION

	// get the mouse position
	int mouse_x, mouse_y;
	Uint32 mouse_state = SDL_GetMouseState(&mouse_x, &mouse_y);

	// use SDL_BUTTON macro to get the mouse state (right and left click)
	m_input.set_mouse_state( SDL_BUTTON(1) & mouse_state, SDL_BUTTON(3) & mouse_state);

	// Quits when ESC is pressed
	if(m_input.get_key(SDL_SCANCODE_ESCAPE))
		m_should_close = true;

}


void Game::run()
{
	auto current_time = steady_clock::now();
	auto previous_time = current_time;

	double delta_time = 0.01;
	double max_dt = 1 / static_cast<double>(m_config.fps);
	int fps_cap = m_config.fps;

	while( !m_states.empty() && !m_should_close )
	{
		// calculate delta_time (as seconds)
		previous_time = current_time;
		current_time = steady_clock::now();
		delta_time = dseconds(current_time - previous_time).count();

		// cap the fps
		if( fps_cap && delta_time < max_dt )
		{
			std::this_thread::sleep_for( dseconds(max_dt - delta_time) );
			delta_time += max_dt - delta_time;
		}

		// get the current state
		State& state = *m_states.back();

		// game clock
		handle_events();
		state.update( m_input, delta_time);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_CULL_FACE);

		state.render();
		SDL_GL_SwapWindow(m_window);

		// state handling
		if(state.should_close())
			m_states.pop_back();
	}
}