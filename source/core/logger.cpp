#include "logger.hpp"

#include <sstream>
#include <iostream>

namespace sd {

	void Logger::send_to_streams( std::initializer_list<std::string> init_list ) {
		std::stringstream ss;

		for(auto &el: init_list)
			ss << el << ' ';
		
		ss << '\n';
		std::cout << ss.str();

	}

}