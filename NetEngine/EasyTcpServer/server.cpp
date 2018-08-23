#include "mem_alloctor.h"
#include "my_server.h"
#include "cell_network.h"

#include <iostream>
#include <string.h>
#include <thread>

std::vector<SOCKET> g_clients;

int port = 10086;

////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////
bool g_runing = true;
void inputThread()
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

int main()
{
	knet::CellNetwork::Instance().Init();
	int thread_count = 4;
	LOG_INFO("input thread count");
	std::cin >> thread_count;
	MyServer server;
	server.CreateSock();
	server.Bind("", port);
	server.Listen(64);
	server.Start(thread_count);

	while (g_runing)
	{
		std::chrono::milliseconds t(100);
		std::this_thread::sleep_for(t);

		char cmdBuff[256] = { 0 };
		LOG_INFO("input a cmd: ");
		std::cin >> cmdBuff;
		if (0 == strcmp(cmdBuff, "exit"))
		{
			g_runing = false;
			server.Close();
			break;
		}
	}
	
	LOG_INFO("Server::Exit!");
	while (1)
	{
		std::chrono::milliseconds t(100);
		std::this_thread::sleep_for(t);

		char cmdBuff[256] = { 0 };
		LOG_INFO("input a cmd: ");
		std::cin >> cmdBuff;
		if (0 == strcmp(cmdBuff, "exit"))
			break;
	}

	knet::CellNetwork::Instance().Fini();

	return 0;
}