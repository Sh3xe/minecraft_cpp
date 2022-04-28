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
