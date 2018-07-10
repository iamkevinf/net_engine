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

int main()
{
	const int count = 3000;
	knet::TCPClient* clients[count];

	for (int i = 0; i < count; ++i)
	{
		if (!g_runing)
			return 0;

		clients[i] = new knet::TCPClient();
	}

	for (int i = 0; i < count; ++i)
	{
		if (!g_runing)
			return 0;

		clients[i]->Conn(host, port);
		std::cout << "Conn Count = " << i << std::endl;
	}

	//std::thread thread_input(inputThread, &client);
	//thread_input.detach();

	std::thread thread_input2(inputThread2);
	thread_input2.detach();

	knet::c2s_Login login;
	strcpy(login.userName, "admin");
	strcpy(login.passWord, "123.com");
	
	while (g_runing)//client.IsRun())
	{
		for (int i = 0; i < count; ++i)
		{
			//clients[i]->OnRun();
			clients[i]->Send(&login);
		}
	}

	for (int i = 0; i < count; ++i)
		clients[i]->CloseSock();

	getchar();
	return 0;
}