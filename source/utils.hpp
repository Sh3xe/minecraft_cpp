#pragma once

#include <string>
#include <utility>

struct no_copy
{
	no_copy() {}
	no_copy(const no_copy&) = delete;
	no_copy operator=(const no_copy&) = delete;
};

std::string trim( const std::string &str );
std::string get_file_content( const std::string &path );
float map( float v, float min1, float max1, float min2, float max2);
float to_01( float v, float m, float M );