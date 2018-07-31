#ifndef __SEMAPHORE_H__
#define __SEMAPHORE_H__

#include <mutex>
#include <condition_variable>

namespace knet
{

	class Semaphore
	{
	public:
		Semaphore();
		~Semaphore();

		void Wait();
		void Weakup();

	private:
		std::condition_variable m_cv;
		std::mutex m_mutex;
		int m_wait_ref = 0;
		int m_weakup_ref = 0;
	};

}; // end of namespace knet

#endif // __SEMAPHORE_H__