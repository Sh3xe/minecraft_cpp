#pragma once

#include "core/logger.hpp"

#include <mutex>
#include <algorithm>
#include <deque>

template <typename T>
class TSQueue
{
public:
	TSQueue()
	{

	}

	~TSQueue()
	{
		
	}

	bool in( const T &val )
	{
		m_mut.lock();
		for(auto &el: m_deq)
			if( el == val )
			{
				m_mut.unlock();
				return true;
			}
		m_mut.unlock();
		return false;
	}

	void push( const T &el )
	{
		m_mut.lock();
		m_deq.push_back(el);
		m_mut.unlock();
	}

	T pop()
	{
		m_mut.lock();
		T el = m_deq.front();
		m_deq.pop_front();
		m_mut.unlock();
		return el;
	}

	int size()
	{
		m_mut.lock();
		int size = m_deq.size();
		m_mut.unlock();
		return size;
	}

	template <typename Func>
	void sort( Func function )
	{
		m_mut.lock();
		std::sort( m_deq.begin(), m_deq.end(), function );
		m_mut.unlock();
	}

private:
	std::deque<T> m_deq;
	std::mutex m_mut;
};