#include "config.hpp"

#include "cmake_defines.hpp"
#include <fstream>

Config &Config::loadFromFile( const std::string &path ) {
	std::ifstream file( ROOT_DIR + path );
	std::string line;

	// read up to the first '=' sign, store the begining in line
	while( std::getline(file, line, '=') ) {
		if( line == "window_width" ) { // test "line"'s value
			file >> window_width;      // get the value
			std::getline(file, line);  // skip the line break

		} else if( line == "window_height" ) {
			file >> window_height;
			std::getline(file, line);

		} else if( line == "window_title" ) {
			std::getline(file, line);
			window_title = line;

		} else
			std::getline(file, line);
		
	}

	file.close();
	return *this;
}