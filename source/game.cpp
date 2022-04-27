#include "game.hpp"

#include "gl_functions.hpp"
#include <SDL_video.h>
#include "states/playing_state.hpp"

Game::Game(const Config &config):
	m_config(config) {

	initWindow();
	m_states.push_back( std::unique_ptr<PlayingState>( new PlayingState(m_config) ) );
	
}

Game::~Game() {
	SDL_DestroyWindow(m_window);
	SDL_GL_DeleteContext(m_context);
}

void Game::initWindow() {
	// init sdl
	SDL_Init( SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_EVENTS );

	// set gl atribs
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	// create window
	m_window = SDL_CreateWindow(
		m_config.window_title.c_str(),
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		m_config.window_width,
		m_config.window_height,
		SDL_WINDOW_OPENGL
	);

	// hide cursor
	SDL_ShowCursor(0);

	// create opengl context
	m_context = SDL_GL_CreateContext(m_window);

	// load glad
	glewInit();

	glClearColor(0.0f, 0.6f, 1.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glViewport(0, 0, m_config.window_width, m_config.window_height);
}

void Game::handle_events() {
	// handles keyboard events
	SDL_Event event;
	while(SDL_PollEvent(&event)) {
		if(event.type == SDL_QUIT)
			m_should_close = true;
		if(event.type == SDL_KEYDOWN)
			m_input.handle_key_down(event);
		if(event.type == SDL_KEYUP)
			m_input.handle_key_up(event);
	}

	// MOUSE MOVEMENT CALCULATION

	// get the mouse position
	int mouse_x, mouse_y;
	Uint32 mouse_state = SDL_GetMouseState(&mouse_x, &mouse_y);

	// use SDL_BUTTON macro to get the mouse state (right and left click)
	m_input.set_click_state( SDL_BUTTON(1) & mouse_state, SDL_BUTTON(3) & mouse_state);

	// get the old position, calculate the new one
	glm::ivec2 old_mouse_pos = m_input.get_mouse_position();

	if (SDL_GetWindowFlags(m_window) & SDL_WINDOW_INPUT_FOCUS) {
		m_input.set_mouse_position(
			old_mouse_pos.x + mouse_x - (m_config.window_width/2),
			old_mouse_pos.y + mouse_y - (m_config.window_height/2) );

		// replace the cursor in the center
		SDL_WarpMouseInWindow(m_window, m_config.window_width/2, m_config.window_height/2);
	}

	// Quits when ESC is pressed
	if(m_input.is_key_pressed(SDL_SCANCODE_ESCAPE))
		m_should_close = true;

}


void Game::run() {
	size_t current_time  = SDL_GetPerformanceCounter(),
		   previous_time = SDL_GetPerformanceCounter();

	double delta_time = 0.0;
	double max_dt = 1.0/(double)m_config.fps_limit;

	while( !m_states.empty() && !m_should_close ) {
		// calculate delta_time (as seconds)
		previous_time = current_time;
		current_time = SDL_GetPerformanceCounter();
		delta_time = (current_time - previous_time) / (double)SDL_GetPerformanceFrequency();

		// cap the fps
		if( m_config.fps_cap && delta_time < max_dt ) {
			SDL_Delay( (max_dt - delta_time) * 1000.0 );
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