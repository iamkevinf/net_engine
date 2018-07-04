#include "tcp_client.h"
#include "message.hpp"

#include <iostream>
#include <thread>
#include <string.h>

#ifdef _WIN32
	std::string host = "192.168.1.102";
#else 
	std::string host = "192.168.35.3";
#endif
uint16_t port = 10086;

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

			client->Send(&login);
		}
		else if (0 == strcmp(cmdBuff, "logout"))
		{
			knet::c2s_Logout logout;
			strcpy(logout.userName, "admin");

			client->Send(&logout);
		}
		else
		{
			std::cout << "not support cmd!" << std::endl;
		}
	}
}

int main()
{
	knet::TCPClient client;
	client.Conn(host, port);

	// input thread
	std::thread thread_input(inputThread, &client);
	thread_input.detach();

	while (client.IsRun())
	{
		client.OnRun();
	}

	client.CloseSock();

	getchar();
	return 0;
}