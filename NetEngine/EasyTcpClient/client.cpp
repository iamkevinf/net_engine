#include "tcp_client.h"
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
std::atomic_int g_sendCount{0};
std::atomic_int g_readyCount{0};

////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////
void inputThread(knet::TCPClient* client)
{
	while (true)
	{
		char cmdBuff[256] = { 0 };
		std::cout << "input a cmd: " << std::endl;
		std::cin >> cmdBuff;

		if (0 == strcmp(cmdBuff, "exit"))
		{
			client->CloseSock();
			break;
		}
		else if (0 == strcmp(cmdBuff, "login"))
		{
			knet::c2s_Login login;
			strcpy(login.userName, "admin");
			strcpy(login.passWord, "123.com");

			client->Send(&login, login.dataLen);
		}
		else if (0 == strcmp(cmdBuff, "logout"))
		{
			knet::c2s_Logout logout;
			strcpy(logout.userName, "admin");

			client->Send(&logout, logout.dataLen);
		}
		else
		{
			std::cout << "not support cmd!" << std::endl;
		}
	}
}

bool g_runing = true;
void inputThread2()
{
	while (true)
	{
		char cmdBuff[256] = { 0 };
		std::cout << "input a cmd: " << std::endl;
		std::cin >> cmdBuff;

		if (0 == strcmp(cmdBuff, "exit"))
		{
			g_runing = false;
			break;
		}
		else
		{
			std::cout << "not support cmd!" << std::endl;
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
	std::cout << "ThreadID = " << thread_id << " Start" << std::endl;

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

	std::cout << "ThreadID = " << thread_id << " Conn Count bng = " << bgn << " end = " << end << std::endl;

	g_readyCount++;
	while (g_readyCount < g_thread_count)
	{
		std::chrono::milliseconds t(10);
		std::this_thread::sleep_for(t);
	}

	// 启动Recv线程
	std::thread t1(RecvFunc, bgn, end);
	t1.detach();

	const int package_count = 10;
	knet::c2s_Heart heart[package_count];
	for (int i = 0; i < package_count; ++i)
	{
	}
	const int nLen = sizeof(heart);

	while (g_runing)//client.IsRun())
	{
		for (int i = bgn; i < end; ++i)
		{
			if (SOCKET_ERROR != g_clients[i]->Send(heart, nLen))
				g_sendCount++;
		}

		std::chrono::milliseconds t(100);
		std::this_thread::sleep_for(t);
	}

	for (int i = bgn; i < end; ++i)
	{
		g_clients[i]->CloseSock();
		delete g_clients[i];
	}

	std::cout << "ThreadID = " << thread_id << " Exit" << std::endl;
}

int main()
{
	std::cout << "please input client connection count:" << std::endl;
	std::cin >> g_client_count;
	for (int i = 0; i < g_client_count; ++i)
		g_clients.push_back(nullptr);

	std::cout << "please input thread count:" << std::endl;
	std::cin >> g_thread_count;

	std::cout << "please input host name:" << std::endl;
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
			std::cout.setf(::std::ios::fixed);
			std::cout << "Thread=" << g_thread_count
				<< " Time=" << ::std::fixed << ::std::setprecision(6) << t
				<< " Conn=" << g_client_count
				<< " Send=" << (int)(g_sendCount/t)
				<< std::endl;

			g_sendCount = 0;

			tTime.Update();

			std::chrono::milliseconds t(1);
			std::this_thread::sleep_for(t);
		}
	}

	getchar();
	return 0;
}