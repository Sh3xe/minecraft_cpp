#include "input.hpp"

void Input::handleKeyUp(const SDL_Event &event) {
	m_keys[event.key.keysym.scancode] = false;
}

void Input::handleKeyDown(const SDL_Event &event) {
	m_keys[event.key.keysym.scancode] = true;
}

void Input::setMousePosition(int x, int y) {
	m_mouse_x = x;
	m_mouse_y = y;
}

void Input::setClickState(bool l, bool r) {
	m_left_click = l;
	m_right_click = r;
}

std::pair<bool, bool> Input::getClickState() {
	return std::pair<bool, bool>(m_left_click, m_right_click);
}


glm::ivec2 Input::getMousePosition() {
    return glm::ivec2(m_mouse_x, m_mouse_y);
}

bool Input::isKeyPressed(SDL_Scancode key) {
	return m_keys[key];
}