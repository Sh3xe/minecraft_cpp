#ifndef INPUT_INCLUDED_
#define INPUT_INCLUDED_

#include <map>
#include <SDL.h>
#include <glm/glm.hpp>

class Input {
public:
	void handleKeyUp(const SDL_Event &event);
	void handleKeyDown(const SDL_Event &event);
	void setMousePosition(int x, int y);
	void setClickState(bool l, bool r);

	glm::ivec2 getMousePosition();
	bool isKeyPressed(SDL_Scancode key);
	std::pair<bool, bool> getClickState();

private:
	std::map<SDL_Scancode, bool> m_keys;
	int m_mouse_x = 0, m_mouse_y = 0;
	bool m_right_click = false, m_left_click = false;
};

#endif