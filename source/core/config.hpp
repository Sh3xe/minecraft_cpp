#pragma once

#include <string>
#include <map>

class Config {
public:
	Config( const std::string &path );

	operator bool() const { return m_is_valid; }

	float get_float( const std::string &name );
	bool get_bool( const std::string &name );
	int get_int( const std::string &name );
	std::string get_str( const std::string &name );

private:
	void parse( const std::string &path );

	bool m_is_valid = true;
	std::map<std::string, std::string> m_str_data;
	std::map<std::string, int> m_int_data;
	std::map<std::string, float> m_float_data;
};