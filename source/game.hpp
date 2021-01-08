#ifndef GAME_INCLUDED_
#define GAME_INCLUDED_

#include <vector>

#include "config.hpp"
#include "states/base_state.hpp"

class Game {
public:
	Game(const Config &config);

	void run();

private:
	std::vector<State> m_states;
	GLFWwindow *m_window;

	void update();
	void render();

};

#endif