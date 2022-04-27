#include "timer.hpp"
#include <iostream>

Timer::Timer()
{
	m_start = std::chrono::high_resolution_clock::now();
}

Timer::Timer(const std::string& msg):
	m_msg(msg)
{
}

Timer::~Timer()
{
	auto end = std::chrono::high_resolution_clock::now();
	std::cout << m_msg << " " << (end - m_start).count() << " ns\n";
}
