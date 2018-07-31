#include "semaphore.h"

#include <iostream>
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
		std::unique_lock<std::mutex> lock(m_mutex);
		if (--m_wait_ref < 0)
		{
			// Spurious Wakeup
			auto func = [this]()->bool 
			{
				return m_weakup_ref > 0;
			};
			m_cv.wait(lock, func);

			m_weakup_ref--;
		}
	}

	void Semaphore::Weakup()
	{
		std::unique_lock<std::mutex> lock(m_mutex);

		if (++m_wait_ref <= 0)
		{
			m_weakup_ref++;
			m_cv.notify_one();
		}
	}

}; // end of namespace knet