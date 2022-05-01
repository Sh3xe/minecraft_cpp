#include "timer.hpp"
#include <iostream>

Timer::Timer()
{
	m_start = std::chrono::high_resolution_clock::now();
}

Timer::Timer(const std::string& msg):
	m_msg(msg)
{
	m_start = std::chrono::high_resolution_clock::now();
}

Timer::~Timer()
{
	auto end = std::chrono::high_resolution_clock::now();
	std::cout << m_msg << " " << std::chrono::duration<float, std::ratio<1, 1000>> (end - m_start).count() << " ms\n";
}
