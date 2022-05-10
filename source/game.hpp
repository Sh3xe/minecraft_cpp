#ifndef GAME_INCLUDED_
#define GAME_INCLUDED_

#include <vector>
#include <memory>

#include "core/config.hpp"
#include "states/base_state.hpp"

// forward defs
struct SDL_Window;
typedef void *SDL_GLContext;

class Game
{
public:
	Game( const Config &config, SDL_Window *window );
	~Game();

	void run();

private:
	void handle_events();

	Config m_config;
	SDL_Window *m_window;
	bool m_should_close = false;
	std::vector< std::unique_ptr<State> > m_states;
};

#endif