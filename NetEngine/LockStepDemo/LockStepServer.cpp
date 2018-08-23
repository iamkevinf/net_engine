#ifdef _WIN32
	#define _CRT_SECURE_NO_WARNINGS
#else
#endif // _WIN32

#include "server.h"
#include "World.h"
#include "cell_network.h"

#include <iostream>
#include <string.h>
#include <thread>


int port = 10086;

////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////

int main()
{
	int thread_count = 1;

	knet::CellNetwork::Instance().Init();

	World::GetInstance()->Start();

	Server server;
	server.CreateSock();
	server.Bind("", port);
	server.Listen(64);
	server.EnableLog(false);
	server.Start(thread_count);
	
	while (true)
	{
		World::GetInstance()->Tick();

		std::chrono::milliseconds t(1);
		std::this_thread::sleep_for(t);
	}

	server.Close();

	knet::CellNetwork::Instance().Fini();

	LOG_INFO("Server::Exit!");
	return 0;
}