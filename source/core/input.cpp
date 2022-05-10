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

void Input::on_click( MouseButton b)
{
    for( auto &[name, fun]: m_callbacks)
        fun(b);
}

void Input::add_click_callback( const Callback &f, const std::string &name )
{
    m_callbacks.insert( std::make_pair(name, f) );
}

void Input::remove_click_callback( const std::string &name )
{
    m_callbacks.erase( name );
}


MouseButton button_from_sdl_enum( uint8_t sdl_enum )
{
    if( sdl_enum == SDL_BUTTON_LEFT )
        return MouseButton::left;
    if( sdl_enum == SDL_BUTTON_RIGHT )
        return MouseButton::right;
    if( sdl_enum == SDL_BUTTON_MIDDLE )
        return MouseButton::middle;
    if( sdl_enum == SDL_BUTTON_X1 )
        return MouseButton::x1;
    if( sdl_enum == SDL_BUTTON_X2 )
        return MouseButton::x2;
    return MouseButton::none;
}
