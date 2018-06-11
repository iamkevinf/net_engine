#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <Windows.h>
#include <WinSock2.h>

#include <string.h>
#include <iostream>

std::string host = "127.0.0.1";
int port = 10086;

enum class MessageType
{
	MT_C2S_LOGIN,
	MT_S2C_LOGIN,

	MT_C2S_LOGOUT,
	MT_S2C_LOGOUT,

	MT_ERROR
};

// 消息头
struct DataHeader
{
	short dataLen; // 数据长度
	MessageType cmd; // 命令
};

struct c2s_Login
{
	char userName[32];
	char passWord[32];
};

struct s2c_Login
{
	int ret;
};

struct c2s_Logout
{
	char userName[32];
};

struct s2c_Logout
{
	int ret;
	char userName[32];
};

int main()
{
	WORD ver = MAKEWORD(2, 2);
	WSADATA data;
	WSAStartup(ver, &data);

	// 1 建立一个socket
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET)
		std::cout << "socket error" << std::endl;

	// 2 连接服务器 connect
	sockaddr_in sin = {};
	sin.sin_family = AF_INET;
	sin.sin_port = htons(port);
	sin.sin_addr.S_un.S_addr = inet_addr(host.c_str());
	if (SOCKET_ERROR == connect(sock, (sockaddr*)&sin, sizeof(sockaddr_in)))
		std::cout << "connect error" << std::endl;
	
	// 处理数据
	const int maxLen = 512;
	char cmdBuff[maxLen] = {};
	while (true)
	{
		std::cout << "input a cmd: " << std::endl;
		std::cin >> cmdBuff;

		if (0 == strcmp(cmdBuff, "exit"))
		{
			break;
		}
		else if(0 == strcmp(cmdBuff, "login"))
		{
			c2s_Login msg = {
				"admin",
				"123.com"
			};
			DataHeader c2sHeader = {
				sizeof(c2s_Login),
				MessageType::MT_C2S_LOGIN
			};

			send(sock, (const char*)&c2sHeader, sizeof(DataHeader), 0);
			send(sock, (const char*)&msg, sizeof(c2s_Login), 0);
			// 接收服务器的返回数据
			DataHeader s2cHeader = {};
			s2c_Login ret = {};
			recv(sock, (char*)&s2cHeader, sizeof(DataHeader), 0);
			recv(sock, (char*)&ret, sizeof(s2c_Login), 0);
			std::cout << "s2c_Login " << ret.ret << std::endl;
		}
		else if (0 == strcmp(cmdBuff, "logout"))
		{
			c2s_Logout msg = {
				"admin"
			};
			DataHeader c2sHeader = {
				sizeof(c2s_Logout),
				MessageType::MT_C2S_LOGOUT
			};

			send(sock, (const char*)&c2sHeader, sizeof(DataHeader), 0);
			send(sock, (const char*)&msg, sizeof(c2s_Logout), 0);
			// 接收服务器的返回数据
			DataHeader s2cHeader = {};
			s2c_Logout ret = {};
			recv(sock, (char*)&s2cHeader, sizeof(DataHeader), 0);
			recv(sock, (char*)&ret, sizeof(s2c_Logout), 0);
			std::cout << "s2c_Logout " << ret.ret << std::endl;
		}
		else
		{
			std::cout << "cmd not support : " << cmdBuff << std::endl;
		}
	}

	// 4 关闭socket
	closesocket(sock);

	WSACleanup();

	system("pause");
	return 0;
}