#include "tcp_server.h"

#include <iostream>
#include <string>

std::vector<SOCKET> g_clients;

std::string host = "192.168.1.102"; //"192.168.35.3";
int port = 10086;

int main()
{
	knet::TCPServer server;
	server.CreateSock();
	server.Bind("", port);
	server.Listen(5);

	while (server.IsRun())
	{
		server.OnRun();
	}

	server.CloseSock();

	getchar();
	return 0;
}