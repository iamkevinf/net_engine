#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <Windows.h>
#include <WinSock2.h>

#include <string.h>
#include <iostream>

std::string host = "127.0.0.1";
int port = 10086;

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

	// 3 接受服务器信息 recv
	const int maxLen = 256;
	char recvBuff[maxLen] = {};
	int nLen = recv(sock, recvBuff, maxLen, 0);
	if (nLen <= 0)
		std::cout << "recv error" << std::endl;

	std::cout << "recv from server: " << recvBuff << std::endl;
	
	// 4 关闭socket
	closesocket(sock);

	WSACleanup();

	system("pause");
	return 0;
}