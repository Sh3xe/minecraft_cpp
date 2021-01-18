#ifndef INPUT_INCLUDED_
#define INPUT_INCLUDED_

#include <map>
#include <SDL.h>

class Input {
public:
	void handleKeyUp(const SDL_Event &event);
	void handleKeyDown(const SDL_Event &event);
	bool isKeyPressed(SDL_Scancode key);

private:
	std::map<SDL_Scancode, bool> m_keys;
};

#endif