#include "config.hpp"

#include "cmake_defines.hpp"
#include "core/logger.hpp"
#include <fstream>
#include <sstream>


Config::Config( const std::string &path ) {

	parse( ROOT_DIR + ( "/" + path) );

	for(auto&e: m_float_data) SD_INFO(e.first, ": ", e.second);
	for(auto&e: m_str_data) SD_INFO(e.first, ": ", e.second);
	for(auto&e: m_int_data) SD_INFO(e.first, ": ", e.second);


}

void Config::parse( const std::string &path ) {

	std::ifstream file { path };

	if(!file) {
		m_is_valid = false;
		return;
	}

	std::string line;
	while( std::getline(file, line) ) {

		if( line.size() < 3 ) {
			SD_WARN("skipped config line");
			continue;
		}

		if( line[0] == '#') continue; // "#" est un commentaire

		// recuperation nom = valeur
		size_t equ_index = line.find_first_of('=');
		if( equ_index == std::string::npos ) {
			SD_WARN("skipped config line");
			continue;
		}

		std::string
			name = line.substr(2, equ_index - 2),
			value = line.substr(equ_index+1);

		// conversion
		char type = line[0];

		std::stringstream ss(value);

		switch( type ) {
			case 'i': {
				int val = 0;
				ss >> val;
				m_int_data.emplace(name, val);
				break;
			}
			case 'f': {
				float val = 0.0f;
				ss >> val;
				m_float_data.emplace(name, val);
				break;
			}
			case 's': {
				m_str_data.emplace(name, value);
				break;
			}
			default:
				SD_WARN("invalid config type");
				break;
		}


	}
}

float Config::get_float( const std::string &name ) {
	return m_float_data[name];
}

bool Config::get_bool( const std::string &name ) {
	return m_int_data[name] >= 1;
}

int Config::get_int( const std::string &name ) {
	return m_int_data[name];
}

std::string Config::get_str( const std::string &name ) {
	return m_str_data[name];
}

void Config::set_float( const std::string &name, float val ) {
	m_float_data[name] = val;
}

void Config::set_bool( const std::string &name, bool val ) {
	m_int_data[name] = static_cast<int>(val);
}

void Config::set_int( const std::string &name, int val ) {
	m_int_data[name] = val;
}

void Config::set_str( const std::string &name, std::string val ) {
	m_str_data[name] = val;
}
