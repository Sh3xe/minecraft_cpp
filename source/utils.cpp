#include "utils.hpp"

std::string trim( const std::string &str )
{
    auto first = str.find_first_not_of(' ');
    auto last = str.find_last_not_of(' ');
    return str.substr(first, last+1);
}

std::string get_file_content( const std::string &path )
{
    return "";
}

float map( float v, float min1, float max1, float min2, float max2)
{
	return min2 + ((v - min1) / (max1 - min1)) * (max2-min2);
}

float to_01( float v, float m, float M )
{
	return (v - m) / (M - m);
}