#ifndef GAME_INCLUDED_
#define GAME_INCLUDED_

#include <vector>
#include <memory>

#include "core/config.hpp"
#include "core/input.hpp"
#include "states/base_state.hpp"

// forward defs
struct SDL_Window;
typedef void *SDL_GLContext;

class Game {
public:
	Game(const Config &config);
	~Game();

	void run();

private:
	void initWindow();
	void handle_events();

	Config m_config;
	Input m_input;
	SDL_Window *m_window;
	SDL_GLContext m_context;
	bool m_should_close = false;
	std::vector< std::unique_ptr<State> > m_states;
};

#endif