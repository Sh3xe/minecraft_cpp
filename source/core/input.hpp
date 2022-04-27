#ifndef INPUT_INCLUDED_
#define INPUT_INCLUDED_

#include <map>
#include <SDL.h>
#include <glm/glm.hpp>

class Input {
public:
	void handle_key_up(const SDL_Event &event);
	void handle_key_down(const SDL_Event &event);
	void set_mouse_position(int x, int y);
	void set_click_state(bool l, bool r);

	glm::ivec2 get_mouse_position();
	bool is_key_pressed(SDL_Scancode key);
	std::pair<bool, bool> get_click_state();

private:
	std::map<SDL_Scancode, bool> m_keys;
	int m_mouse_x = 0, m_mouse_y = 0;
	bool m_right_click = false, m_left_click = false;
};

#endif