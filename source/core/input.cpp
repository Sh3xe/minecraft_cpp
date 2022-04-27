#include "input.hpp"

void Input::handle_key_up(const SDL_Event &event) {
	m_keys[event.key.keysym.scancode] = false;
}

void Input::handle_key_down(const SDL_Event &event) {
	m_keys[event.key.keysym.scancode] = true;
}

void Input::set_mouse_position(int x, int y) {
	m_mouse_x = x;
	m_mouse_y = y;
}

void Input::set_click_state(bool l, bool r) {
	m_left_click = l;
	m_right_click = r;
}

std::pair<bool, bool> Input::get_click_state() {
	return std::pair<bool, bool>(m_left_click, m_right_click);
}


glm::ivec2 Input::get_mouse_position() {
    return glm::ivec2(m_mouse_x, m_mouse_y);
}

bool Input::is_key_pressed(SDL_Scancode key) {
	return m_keys[key];
}