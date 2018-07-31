#include "mem_alloctor.h"
#include "my_server.h"

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

int main()
{
	MyServer server;
	server.CreateSock();
	server.Bind("", port);
	server.Listen(5);
	server.Start(4);

	while (g_runing)
	{
		std::chrono::milliseconds t(100);
		std::this_thread::sleep_for(t);

		char cmdBuff[256] = { 0 };
		std::cout << "input a cmd: " << std::endl;
		std::cin >> cmdBuff;
		if (0 == strcmp(cmdBuff, "exit"))
		{
			g_runing = false;
			server.Close();
			break;
		}
	}
	
	return 0;
}