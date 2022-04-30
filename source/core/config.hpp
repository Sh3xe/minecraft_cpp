#pragma once

#include <cstdint>
#include <string>

struct Config
{
	std::string title;
	int window_width, window_height;
	bool fps_cap;
	int fps;
	float sensitivity;
	bool fullscreen;
};

Config load_config_from_file( const std::string &path );