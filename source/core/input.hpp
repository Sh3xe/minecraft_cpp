#pragma once

#include <map>
#include <SDL.h>

#include <stdint.h>

class Input
{
public:
	inline bool get_key(SDL_Scancode key) { return m_keys[key]; }
	std::pair<bool, bool> get_mouse_state();

	void handle_keyup( SDL_Scancode key );
	void handle_keydown( SDL_Scancode key );
	void set_mouse_pos(int32_t x, int32_t y);
	void add_mouse_pos(int32_t x, int32_t y);
	void set_mouse_state( bool rc, bool lc );

	inline const int32_t get_mouse_x() const { return m_mouse_x; }
	inline const int32_t get_mouse_y() const { return m_mouse_y; }

private:
	std::map<SDL_Scancode, bool> m_keys;
	std::pair<bool, bool> m_click_state;
	int32_t m_mouse_x = 0.0f, m_mouse_y = 0.0f;

};