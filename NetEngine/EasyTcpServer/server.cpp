#include "tcp_server.h"

#include <iostream>
#include <string.h>
#include <thread>

std::vector<SOCKET> g_clients;

#ifdef _WIN32
std::string host = "192.168.1.102";
#else 
	std::string host = "192.168.35.3";
#endif
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
	knet::TCPServer server;
	server.CreateSock();
	server.Bind("", port);
	server.Listen(5);
	server.Start();

	std::thread input(inputThread);
	input.detach();

	while (server.IsRun() && g_runing)
	{
		server.OnRun();
	}

	server.CloseSock();

	getchar();
	return 0;
}