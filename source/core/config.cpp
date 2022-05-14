#include "config.hpp"

#include <iostream>
#include <fstream>
#include <sstream>


Config::Config( const std::string &path )
{
	parse(path);
}

void Config::parse( const std::string &path )
{

	std::ifstream file { path };

	if(!file)
	{
		m_is_valid = false;
		return;
	}

	std::string line;
	while( std::getline(file, line) )
	{

		if( line.size() < 3 ) continue;

		// getting the index of the equal symbol
		size_t equ_index = line.find_first_of('=');
		if( equ_index == std::string::npos ) continue;

		std::string
			name = line.substr(2, equ_index - 2),
			value = line.substr(equ_index+1);

		// conversion
		char type = line[0];

		std::stringstream ss(value);

		switch( type )
		{
			case 'i':
			{
				int val = 0;
				ss >> val;
				m_int_data.emplace(name, val);
				break;
			}
			case 'f':
			{
				float val = 0.0f;
				ss >> val;
				m_float_data.emplace(name, val);
				break;
			}
			case 's':
			{
				m_str_data.emplace(name, value);
				break;
			}
			default: break;
		}
	}
}

float Config::get_float( const std::string &name )
{
	return m_float_data.at(name);
}

bool Config::get_bool( const std::string &name )
{
	return m_int_data.at(name) >= 1;
}

int Config::get_int( const std::string &name )
{
	return m_int_data.at(name);
}

std::string Config::get_str( const std::string &name )
{
	return m_str_data.at(name);
}