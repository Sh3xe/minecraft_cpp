#pragma once

#include <string>
#include <utility>

std::string trim( const std::string &str );
std::string get_file_content( const std::string &path );
float map( float v, float min1, float max1, float min2, float max2);
float to_01( float v, float m, float M );