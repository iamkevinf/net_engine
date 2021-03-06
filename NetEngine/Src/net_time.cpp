﻿#include "net_time.h"

namespace knet
{

	Time::Time()
	{
		m_begin = GetTime();
	}

	Time::~Time()
	{

	}

	void Time::Update()
	{
		m_begin = GetTime();
	}

	std::chrono::time_point<std::chrono::high_resolution_clock>  Time::GetTime()
	{
		return std::chrono::high_resolution_clock::now();
	}

	double Time::GetElapsedSecond()
	{
		return GetElapsedTimeMicroSec() * 0.000001;
	}

	double Time::GetElapsedTimeMS()
	{
		return GetElapsedTimeMicroSec() * 0.001;
	}

	long long Time::GetElapsedTimeMicroSec()
	{
		return std::chrono::duration_cast<std::chrono::microseconds>(GetTime() - m_begin).count();
	}

	/*static*/ time_t Time::GetCurTime()
	{
		return std::chrono::duration_cast<std::chrono::milliseconds>(
			std::chrono::high_resolution_clock::now().time_since_epoch())
			.count();
	}

}; // end of namespace knet