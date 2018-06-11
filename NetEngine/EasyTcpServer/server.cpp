#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <Windows.h>
#include <WinSock2.h>

#include <iostream>
#include <string>


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

// ��Ϣͷ
struct DataHeader
{
	short dataLen; // ���ݳ���
	MessageType cmd; // ����
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

	while (true)
	{
		DataHeader header = {};

		// ���ܿͻ�����������
		int nLenRecv = recv(clientSock, (char*)&header, sizeof(DataHeader), 0);
		if (nLenRecv <= 0)
		{
			std::cout << "client exit!" << std::endl;
			break;
		}

		std::cout << "recv cmd: " << (int)header.cmd << " len: " << header.dataLen << std::endl;

		switch (header.cmd)
		{
		case MessageType::MT_C2S_LOGIN:
		{
			c2s_Login login = {};
			recv(clientSock, (char*)&login, sizeof(c2s_Login), 0);
			s2c_Login ret = {100};
			send(clientSock, (char*)&header, sizeof(DataHeader), 0);
			send(clientSock, (char*)&ret, sizeof(s2c_Login), 0);
		}
			break;

		case MessageType::MT_C2S_LOGOUT:
		{
			c2s_Logout logout = {};
			recv(clientSock, (char*)&logout, sizeof(c2s_Logout), 0);
			s2c_Logout ret = { 100 };
			send(clientSock, (char*)&header, sizeof(DataHeader), 0);
			send(clientSock, (char*)&ret, sizeof(s2c_Logout), 0);
		}
			break;

		default:
		{
			header.cmd = MessageType::MT_ERROR;
			header.dataLen = 0;
			send(clientSock, (char*)&header, sizeof(DataHeader), 0);
		}
			break;
		}
	}

	// 6.�ر�socket
	closesocket(sock);

	WSACleanup();
	system("pause");
	return 0;
}