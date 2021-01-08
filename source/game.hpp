#ifndef GAME_INCLUDED_
#define GAME_INCLUDED_

#include <vector>
#include <memory>

#include "opengl_wrapper/render_context.hpp"
#include "config.hpp"
#include "states/base_state.hpp"

class Game {
public:
	Game(const Config &config);

	void run();

private:
	std::vector< std::unique_ptr<State> > m_states;
	RenderContext m_context;

	void update();
	void render();

};

#endif