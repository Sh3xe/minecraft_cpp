#include "config.hpp"

#include "cmake_defines.hpp"
#include <fstream>

Config &Config::loadFromFile( const std::string &path ) {
	std::ifstream file( ROOT_DIR + path );
	std::string line;

	std::getline(file, window_title);

	file >> window_width >> window_height;
	file >> fps_limit >> fps_cap;
	file >> sensitivity;

	file.close();
	return *this;
}