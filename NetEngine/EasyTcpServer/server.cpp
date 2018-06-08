#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <Windows.h>
#include <WinSock2.h>

#include <iostream>
#include <string>


std::string host = "127.0.0.1";
int port = 10086;

struct DataPackage
{
	int age;
	char name[32];
};

int main()
{
	WORD ver = MAKEWORD(2, 2);
	WSADATA data;
	WSAStartup(ver, &data);

	// 1����socket
	SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET)
		std::cout << "socket error" << std::endl;

	// 2 bind�����ڽ��ܿͻ������ӵ�����˿�
	sockaddr_in _sin = {};
	_sin.sin_family = AF_INET;
	_sin.sin_port = htons(port);
	_sin.sin_addr.S_un.S_addr = INADDR_ANY; // inet_addr(host.c_str());
	if ( SOCKET_ERROR == bind(sock, (sockaddr*)&_sin, sizeof(sockaddr_in)) )
		std::cout << "bind error" << std::endl;

	// 3 listen ��������˿�
	if (SOCKET_ERROR == listen(sock, 5))
		std::cout << "listen error" << std::endl;

	// 4 accept �ȴ��ͻ�������
	sockaddr_in clientAddr = {};
	int nAddrLen = sizeof(sockaddr_in);

	SOCKET clientSock = INVALID_SOCKET;
	
	clientSock = accept(sock, (sockaddr*)&clientAddr, &nAddrLen);
	if (clientSock == INVALID_SOCKET)
		std::cout << "accpet error: invalid client" << std::endl;

	std::cout << "new client: connection, sock = " << sock << "IP = " << inet_ntoa(clientAddr.sin_addr) << std::endl;

	const int maxRecvLen = 512;
	char recvBuff[maxRecvLen] = {};
	
	while (true)
	{
		// ���ܿͻ�����������
		int nLenRecv = recv(clientSock, recvBuff, maxRecvLen, 0);
		if (nLenRecv <= 0)
		{
			std::cout << "client exit!" << std::endl;
			break;
		}

		std::cout << "recv cmd: " << recvBuff << std::endl;

		// ����ͻ�������
		if (0 == strcmp(recvBuff, "getInfo"))
		{
			DataPackage data = { 18, "����" };
			send(clientSock, (const char*)&data, sizeof(DataPackage), 0);
		}
		else
		{
			char msgBuff[] = "???";
			send(clientSock, msgBuff, strlen(msgBuff) + 1, 0);
		}
;
	}

	// 6.�ر�socket
	closesocket(sock);

	WSACleanup();
	system("pause");
	return 0;
}