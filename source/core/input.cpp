#include "input.hpp"

void Input::set_mouse_state( bool rc, bool lc )
{
    m_click_state.first = rc;
    m_click_state.second = lc;
}

std::pair<bool, bool> Input::get_mouse_state()
{
    return m_click_state;
}

void Input::handle_keyup( SDL_Scancode key )
{
    m_keys[key] = false;
}

void Input::handle_keydown( SDL_Scancode key )
{
    m_keys[key] = true;
}

void Input::set_mouse_pos(int32_t x, int32_t y)
{
    m_mouse_x = x;
    m_mouse_y = y;
}

void Input::add_mouse_pos(int32_t x, int32_t y)
{
    m_mouse_x += x;
    m_mouse_y += y;
}