#include "mem_alloctor.h"

#include <iostream>
#include <thread>
#include <mutex>
#include <atomic>

#include "net_time.h"

// 原子操作 也就是不可分割的操作
std::atomic_int g_atomic_sum = 0;

std::mutex g_mutex;

const int tCount = 8;
const int maxAllocCount = 1000000;
const int preAllocCount = maxAllocCount / tCount;
int g_sum = 0;

void thread_func(int index)
{
	char* data[preAllocCount];
	for (size_t i = 0; i < preAllocCount; ++i)
	{
		data[i] = new char[rand() % 1024 + 1];
	}
	for (size_t i = 0; i < preAllocCount; ++i)
	{
		delete[] data[i];
	}


	for (int i = 0; i < 20000000; ++i)
	{
		//g_atomic_sum++;
		//std::lock_guard<std::mutex> guard(g_mutex);
		////g_mutex.lock();
		//	g_sum++;
		////g_mutex.unlock();
	}
}

int main()
{
	knet::Time t1;
	std::thread t[tCount];
	for (int i = 0; i < tCount; ++i)
	{
		t[i] = std::thread(thread_func, i);
	}
	double tt1 = t1.GetElapsedTimeMS();
	for (int i = 0; i < tCount; ++i)
	{
		t[i].join();
		//t[i].detach();
	}
	double tt2 = t1.GetElapsedTimeMS();
	std::cout << "In Job  Thread g_sum = " << g_sum << " dTime = " << tt2 - tt1 << std::endl;

	getchar();
	return 0;
}