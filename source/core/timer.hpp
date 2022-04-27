#pragma once

#include <string>
#include <chrono>

class Timer
{
public:
	Timer();
	Timer( const std::string &msg );
	~Timer();

private:
	std::chrono::high_resolution_clock::time_point m_start;
	std::string m_msg;

};
