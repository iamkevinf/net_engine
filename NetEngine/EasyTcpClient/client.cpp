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

	MT_S2C_JOIN,

	MT_ERROR
};

// 消息头
struct DataHeader
{
	short dataLen; // 数据长度
	MessageType cmd; // 命令
};

struct c2s_Login : public DataHeader
{
	c2s_Login()
	{
		dataLen = sizeof(c2s_Login);
		cmd = MessageType::MT_C2S_LOGIN;
	}

	char userName[32];
	char passWord[32];
};

struct s2c_Login : public DataHeader
{
	s2c_Login()
	{
		dataLen = sizeof(s2c_Login);
		cmd = MessageType::MT_S2C_LOGIN;
		ret = 0;
	}

	int ret;
};

struct c2s_Logout : public DataHeader
{
	c2s_Logout()
	{
		dataLen = sizeof(c2s_Logout);
		cmd = MessageType::MT_C2S_LOGOUT;
	}

	char userName[32];
};

struct s2c_Logout : public DataHeader
{
	s2c_Logout()
	{
		dataLen = sizeof(s2c_Logout);
		cmd = MessageType::MT_S2C_LOGOUT;
		ret = 0;
	}

	int ret;
	char userName[32];
};

struct s2c_Join : public DataHeader
{
	s2c_Join()
	{
		dataLen = sizeof(s2c_Join);
		cmd = MessageType::MT_S2C_JOIN;
		sock = 0;
	}

	int sock;
};


int processor(SOCKET sock)
{
	const int headerSize = sizeof(DataHeader);

	char szRecv[1024] = {};
	// 接受客户端请求数据
	int nLenRecv = recv(sock, szRecv, headerSize, 0);
	DataHeader* header = (DataHeader*)szRecv;
	if (nLenRecv <= 0)
	{
		std::cout << "disconnection from server" << std::endl;
		return -1;
	}

	switch (header->cmd)
	{
	case MessageType::MT_S2C_LOGIN:
	{
		recv(sock, szRecv + headerSize, header->dataLen - headerSize, 0);
		s2c_Login* ret = (s2c_Login*)szRecv;
		std::cout << "s2c_Login " << ret->ret << std::endl;
	}
	break;

	case MessageType::MT_C2S_LOGOUT:
	{
		recv(sock, szRecv + headerSize, header->dataLen - headerSize, 0);
		s2c_Logout* ret = (s2c_Logout*)szRecv;
		std::cout << "s2c_Logout " << ret->ret << std::endl;

	}
	break;

	case MessageType::MT_S2C_JOIN:
	{
		recv(sock, szRecv + headerSize, header->dataLen - headerSize, 0);
		s2c_Join* ret = (s2c_Join*)szRecv;
		std::cout << "s2c_Join " << ret->sock << std::endl;
	}
	break;

	}

	return 0;
}

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
	
	c2s_Login msg;
	strcpy_s(msg.userName, "admin");
	strcpy_s(msg.passWord, "123.com");
	send(sock, (const char*)&msg, sizeof(c2s_Login), 0);

	// 处理数据
	const int maxLen = 512;
	char cmdBuff[maxLen] = {};
	while (true)
	{
		fd_set fdReads;
		FD_ZERO(&fdReads);
		FD_SET(sock, &fdReads);
		int ret = select(sock, &fdReads, 0, 0, 0);
		if (ret < 0)
		{
			std::cout << "select over" << std::endl;
			break;
		}

		if (FD_ISSET(sock, &fdReads))
		{
			FD_CLR(sock, &fdReads);

			if (-1 == processor(sock))
			{
				std::cout << "select over" << std::endl;
				break;
			}
		}
	}

	// 4 关闭socket
	closesocket(sock);

	WSACleanup();

	system("pause");
	return 0;
}