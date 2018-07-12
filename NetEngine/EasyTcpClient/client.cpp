#include "tcp_client.h"
#include "message.hpp"

#include <iostream>
#include <thread>
#include <vector>
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

int g_client_count = 2000;
int g_thread_count = 8;
std::vector<knet::TCPClient*> g_clients;

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

void SendFunc(int thread_id)
{
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
		std::cout << "ThreadID = " << thread_id << " Conn Count = " << i << std::endl;
	}

	const int package_count = 10;
	knet::c2s_Login login[package_count];
	for (int i = 0; i < package_count; ++i)
	{
		strcpy(login[i].userName, "admin");
		strcpy(login[i].passWord, "123.com");
	}
	const int nLen = sizeof(login);

	std::chrono::milliseconds t(5000);
	std::this_thread::sleep_for(t);

	while (g_runing)//client.IsRun())
	{
		for (int i = bgn; i < end; ++i)
		{
			g_clients[i]->Send(login, nLen);
			//clients[i]->OnRun();
		}
	}

	for (int i = bgn; i < end; ++i)
		g_clients[i]->CloseSock();

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

	while (true)
	{
		Sleep(100);
	}

	getchar();
	return 0;
}