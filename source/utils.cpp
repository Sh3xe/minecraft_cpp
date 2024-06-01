#include "utils.hpp"
#include "core/logger.hpp"

#include <fstream>
#include <iterator>

std::string trim( const std::string &str )
{
    auto first = str.find_first_not_of(' ');
    auto last = str.find_last_not_of(' ');
    return str.substr(first, last+1);
}

std::string get_file_content( const std::string &path )
{
    std::fstream file{ path };

	if(!file)
	{
		SD_WARN("impossible de récuperer le contenu de: ", path, "\n");
		return "";
	}

	file << std::noskipws;
	std::string source { std::istream_iterator<char>(file), std::istream_iterator<char>() };
    file.close();
    
    return source;
}

/**
 * @brief map the value v from the range [vmin1, vmax1] to [vmin2, vmax2]
 */
float map( float v, float min1, float max1, float min2, float max2)
{
	return min2 + ((v - min1) / (max1 - min1)) * (max2-min2);
}

/**
 * @brief map the value v from the range [m, M] to [0,1]
 */
float to_01( float v, float vmin, float vmax )
{
	return (v - vmin) / (vmax - vmin);
}