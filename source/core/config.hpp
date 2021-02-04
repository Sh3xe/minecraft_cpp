#ifndef CONFIG_INCLUDED_
#define CONFIG_INCLUDED_

#include <string>

struct Config {
	int window_width         = 1280;
	int window_height        = 720;
	int fps_limit            = 60;
	bool fps_cap             = true;
	bool fog_enabled         = false;
	double sensitivity       = 0.005;
	std::string window_title = "Window";

	Config &loadFromFile( const std::string &path );
};

#endif