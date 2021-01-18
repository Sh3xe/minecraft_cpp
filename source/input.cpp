#include "input.hpp"

void Input::handleKeyUp(const SDL_Event &event) {
	m_keys[event.key.keysym.scancode] = false;
}

void Input::handleKeyDown(const SDL_Event &event) {
	m_keys[event.key.keysym.scancode] = true;
}

bool Input::isKeyPressed(SDL_Scancode key) {
	return m_keys[key];
}