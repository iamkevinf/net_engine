﻿#include "tcp_client.h"
#include "message.hpp"
#include "net_time.h"

#include <iostream>
#include <thread>
#include <vector>
#include <string.h>
#include <atomic>
#include <iomanip>

#ifdef _WIN32
std::string host = "192.168.1.102";
#else 
std::string host = "192.168.35.3";
#endif

uint16_t port = 10086;
////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////

int g_client_count = 2000;
int g_thread_count = 8;
std::vector<knet::TCPClient*> g_clients;
std::atomic_int g_sendCount{ 0 };
std::atomic_int g_readyCount{ 0 };

////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////
bool g_runing = true;
void inputThread2()
{
	while (true)
	{
		char cmdBuff[256] = { 0 };
		LOG_INFO("input a cmd: ");
		std::cin >> cmdBuff;

		if (0 == strcmp(cmdBuff, "exit"))
		{
			g_runing = false;
			break;
		}
		else
		{
			LOG_ERROR("not support cmd!");
		}
	}
}

void RecvFunc(int bgn, int end)
{
	knet::Time t;
	while (g_runing)//client.IsRun())
	{
		for (int i = bgn; i < end; ++i)
		{
			if (t.GetElapsedSecond() > 3 && i == bgn)
				continue;

			g_clients[i]->OnRun();
		}
	}
}

void SendFunc(int thread_id)
{
	LOG_INFO("ThreadID = %d Start", thread_id);

	// thread_id 1 base
	int pre_count = g_client_count / g_thread_count;
	int bgn = (thread_id - 1) * pre_count;
	int end = thread_id * pre_count;

	for (int i = bgn; i < end; ++i)
	{
		if (!g_runing)
			return;

		g_clients[i] = new knet::TCPClient();
	}

	for (int i = bgn; i < end; ++i)
	{
		if (!g_runing)
			return;

		g_clients[i]->Conn(host, port);
	}

	LOG_INFO("ThreadID = %d Conn Count bgn= %d end = %d", thread_id, bgn, end);

	g_readyCount++;
	while (g_readyCount < g_thread_count)
	{
		std::chrono::milliseconds t(10);
		std::this_thread::sleep_for(t);
	}

	// 启动Recv线程
	std::thread t1(RecvFunc, bgn, end);
	t1.detach();

	knet::MessageBody message;
	int size = 4;
	message.size = size + knet::MessageBody::HEADER_TYPE_BYTES;
	message.type = 8;

	while (g_runing)//client.IsRun())
	{
		for (int i = bgn; i < end; ++i)
		{
			int tmp = rand();
			::memcpy(message.data, &tmp, size);

			//if (SOCKET_ERROR != g_clients[i]->Send(heart, nLen))
			if (SOCKET_ERROR != g_clients[i]->Send(&message))
				g_sendCount++;
			//if (SOCKET_ERROR != g_clients[i]->Send(body, nLenBody))
			//	g_sendCount++;
		}

		std::chrono::milliseconds t(100);
		std::this_thread::sleep_for(t);
	}

	for (int i = bgn; i < end; ++i)
	{
		g_clients[i]->CloseSock();
		delete g_clients[i];
	}

	LOG_INFO("ThreadID = %d Exit", thread_id);
}

int main()
{
	LOG_INFO("please input client connection count:");
	std::cin >> g_client_count;
	for (int i = 0; i < g_client_count; ++i)
		g_clients.push_back(nullptr);

	LOG_INFO("please input thread count:");
	std::cin >> g_thread_count;

	LOG_INFO("please input host name:");
	std::cin >> host;

	std::thread thread_input(inputThread2);
	thread_input.detach();

	for (int i = 0; i < g_thread_count; ++i)
	{
		std::thread t(SendFunc, i + 1);
		t.detach();
	}

	knet::Time tTime;
	while (g_runing)
	{
		auto t = tTime.GetElapsedSecond();
		if (t >= 1.0)
		{
			LOG_INFO("Thread=%d Time=%f Conn=%d Send=%d", g_thread_count, t, g_client_count, (int)g_sendCount / t);

			g_sendCount = 0;

			tTime.Update();

			std::chrono::milliseconds t(1);
			std::this_thread::sleep_for(t);
		}
	}

	getchar();
	return 0;
}