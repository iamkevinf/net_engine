#ifndef __SEMAPHORE_H__
#define __SEMAPHORE_H__

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
		bool m_isWaitToExit = false;
	};

}; // end of namespace knet

#endif // __SEMAPHORE_H__