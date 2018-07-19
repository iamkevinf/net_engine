#include <iostream>
#include <thread>
#include <mutex>
#include <atomic>

#include "net_time.h"
#include "mem_alloctor.h"

// 原子操作 也就是不可分割的操作
std::atomic_int g_atomic_sum = 0;

std::mutex g_mutex;

const int tCount = 4;

int g_sum = 0;

void thread_func(int index)
{
	for (int i = 0; i < 20000000; ++i)
	{
		g_atomic_sum++;
		//std::lock_guard<std::mutex> guard(g_mutex);
		////g_mutex.lock();
		//	g_sum++;
		////g_mutex.unlock();
	}
}

int main()
{
	//knet::Time t1;
	//std::thread t[tCount];
	//for (int i = 0; i < tCount; ++i)
	//{
	//	t[i] = std::thread(thread_func, i);
	//}
	//double tt1 = t1.GetElapsedTimeMS();
	//for (int i = 0; i < tCount; ++i)
	//{
	//	t[i].join();
	//	//t[i].detach();
	//}
	//double tt2 = t1.GetElapsedTimeMS();
	//g_sum = 0;
	//for (int i = 0; i < 20000000*tCount; ++i)
	//{
	//	g_sum++;
	//}
	//double tt3 = t1.GetElapsedTimeMS();
	//std::cout << "In Job  Thread g_sum = " << g_sum << " dTime = " << tt2 - tt1 << std::endl;
	//std::cout << "In Main Thread g_sum = " << g_sum << " dTime = " << tt3 - tt2 << std::endl;
	//std::cout << "Hello, main thread" << std::endl;

	//char* data3 = new char[128];
	//char* data1 = new char;
	//char* data2 = new char[64];

	//delete[] data3;
	//delete data1;
	//delete[] data2;
	
	char* data[12];
	for (size_t i = 0; i < 12; ++i)
	{
		data[i] = new char[12];
	}

	for (size_t i = 0; i < 12; ++i)
	{
		delete[] data[i];
	}

	getchar();
	return 0;
}