#ifndef __NET_TIME_H__
#define __NET_TIME_H__

#include <chrono>

namespace knet
{

	class Time
	{
	public:
		Time();
		virtual ~Time();

		void Update();

		std::chrono::time_point<std::chrono::high_resolution_clock> GetTime();

		double GetElapsedSecond();
		double GetElapsedTimeMS();
		long long GetElapsedTimeMicroSec();

	private:
		std::chrono::time_point<std::chrono::high_resolution_clock> m_begin;
	};

}; // end of namespace knet

#endif // __TIMER_H__