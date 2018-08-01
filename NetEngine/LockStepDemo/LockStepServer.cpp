﻿#include "server.h"

#include <iostream>
#include <string.h>
#include <thread>

int port = 10010;

////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////

int main()
{
	int thread_count = 1;

	Server server;
	server.CreateSock();
	server.Bind("", port);
	server.Listen(64);
	server.EnableLog(false);
	server.Start(thread_count);
	
	while (true)
	{
		if(getchar() == 'q')
			server.Close();
	}

	std::cout << "Server::Exit!" << std::endl;
	return 0;
}