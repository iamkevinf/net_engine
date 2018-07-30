#include "semaphore.h"

#include <thread>

namespace knet
{

	Semaphore::Semaphore()
	{

	}

	Semaphore::~Semaphore()
	{

	}

	void Semaphore::Wait()
	{
		m_isWaitToExit = true;
		while (m_isWaitToExit)
		{
			std::chrono::milliseconds t(1);
			std::this_thread::sleep_for(t);
		}
	}

	void Semaphore::Weakup()
	{
		m_isWaitToExit = false;
	}

}; // end of namespace knet