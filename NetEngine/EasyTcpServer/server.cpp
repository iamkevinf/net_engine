#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <Windows.h>
#include <WinSock2.h>

#include <iostream>
#include <string>
#include <vector>

std::vector<SOCKET> g_clients;

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

int processor(SOCKET cSock)
{
	const int headerSize = sizeof(DataHeader);

	char szRecv[1024] = {};
	// 接受客户端请求数据
	int nLenRecv = recv(cSock, szRecv, headerSize, 0);
	DataHeader* header = (DataHeader*)szRecv;
	if (nLenRecv <= 0)
	{
		std::cout << "client <Socket=" << cSock << "> exit!" << std::endl;
		return -1;
	}

	switch (header->cmd)
	{
	case MessageType::MT_C2S_LOGIN:
	{
		recv(cSock, szRecv + headerSize, header->dataLen - headerSize, 0);
		c2s_Login* login = (c2s_Login*)szRecv;

		std::cout << "recv " << "<Socket=" << cSock << "> cmd: " << (int)header->cmd << " len: " << login->dataLen << " username: " << login->userName << " password: " << login->passWord << std::endl;

		s2c_Login ret;
		ret.ret = 100;
		send(cSock, (char*)&ret, sizeof(s2c_Login), 0);
	}
	break;

	case MessageType::MT_C2S_LOGOUT:
	{
		recv(cSock, szRecv + headerSize, header->dataLen - headerSize, 0);
		c2s_Logout* logout = (c2s_Logout*)szRecv;

		std::cout << "recv " << "<Socket=" << cSock << "> cmd: " << (int)header->cmd << " len: " << logout->dataLen << " username: " << logout->userName << std::endl;

		s2c_Logout ret;
		ret.ret = 100;
		send(cSock, (char*)&ret, sizeof(s2c_Logout), 0);
	}
	break;

	default:
	{
		header->cmd = MessageType::MT_ERROR;
		header->dataLen = 0;
		send(cSock, (char*)&header, sizeof(DataHeader), 0);
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

	// 1建立socket
	SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET)
		std::cout << "socket error" << std::endl;

	// 2 bind绑定用于接受客户端连接的网络端口
	sockaddr_in _sin = {};
	_sin.sin_family = AF_INET;
	_sin.sin_port = htons(port);
	_sin.sin_addr.S_un.S_addr = INADDR_ANY; // inet_addr(host.c_str());
	if (SOCKET_ERROR == bind(sock, (sockaddr*)&_sin, sizeof(sockaddr_in)))
		std::cout << "bind error" << std::endl;

	// 3 listen 监听网络端口
	if (SOCKET_ERROR == listen(sock, 5))
		std::cout << "listen error" << std::endl;

	int maxsock = sock;
	while (true)
	{
		fd_set fdRead;
		fd_set fdWrite;
		fd_set fdExp;

		FD_ZERO(&fdRead);
		FD_ZERO(&fdWrite);
		FD_ZERO(&fdExp);

		FD_SET(sock, &fdRead);
		FD_SET(sock, &fdWrite);
		FD_SET(sock, &fdExp);

		for (int n = (int)g_clients.size() - 1; n >= 0; n--)
		{
			FD_SET(g_clients[n], &fdRead);
		}

		timeval t = { 1,0 };
		int ret = select(maxsock + 1, &fdRead, &fdWrite, &fdExp, &t);
		if (ret < 0)
		{
			std::cout << "select over" << std::endl;
			break;
		}

		if (FD_ISSET(sock, &fdRead))
		{
			FD_CLR(sock, &fdRead);

			// 4 accept 等待客户端连接
			sockaddr_in clientAddr = {};
			int nAddrLen = sizeof(sockaddr_in);

			SOCKET clientSock = INVALID_SOCKET;
			clientSock = accept(sock, (sockaddr*)&clientAddr, &nAddrLen);
			if (clientSock == INVALID_SOCKET)
				std::cout << "accpet error: invalid client" << std::endl;

			for (int n = (int)g_clients.size() - 1; n >= 0; n--)
			{
				s2c_Join msg;
				msg.sock = g_clients[n];
				send(g_clients[n], (const char*)&msg, sizeof(s2c_Join), 0);
			}

			g_clients.push_back(clientSock);
			std::cout << "new client: connection, sock = " << clientSock << " IP = " << inet_ntoa(clientAddr.sin_addr) << std::endl;
		}

		for (size_t n = 0; n < fdRead.fd_count; ++n)
		{
			if (-1 == processor(fdRead.fd_array[n]))
			{
				auto iter = find(g_clients.begin(), g_clients.end(), fdRead.fd_array[n]);
				if (iter != g_clients.end())
				{
					g_clients.erase(iter);
				}
			}
		}

		std::cout << "other..." << std::endl;
	}

	for (SOCKET ele : g_clients)
	{
		closesocket(ele);
	}

	// 6.关闭socket
	closesocket(sock);

	WSACleanup();
	system("pause");
	return 0;
}