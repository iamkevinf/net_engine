#include "tcp_server.h"
#include "message.hpp"

#include <iostream>

namespace knet
{

	TCPServer::TCPServer()
	{
	}

	TCPServer::~TCPServer()
	{
		CloseSock();
	}

	void TCPServer::CreateSock()
	{
#ifdef _WIN32
		WORD ver = MAKEWORD(2, 2);
		WSADATA data;
		WSAStartup(ver, &data);
#endif

		if (m_sock != INVALID_SOCKET)
		{
			std::cout << "close old conn" << std::endl;
			CloseSock();
		}

		m_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (m_sock == INVALID_SOCKET)
			std::cout << "CreateSock Error" << std::endl;

		std::cout << "CreateSock Done <Sock:" << m_sock << ">" << std::endl;
	}

	int TCPServer::Bind(const std::string& ip, uint16_t port)
	{
		if (INVALID_SOCKET == m_sock)
			CreateSock();

		sockaddr_in _sin = {};
		_sin.sin_family = AF_INET;
		_sin.sin_port = htons(port);
#ifdef _WIN32
		_sin.sin_addr.S_un.S_addr = ip.empty() ? INADDR_ANY : inet_addr(ip.c_str());
#else
		_sin.sin_addr.s_addr = ip.empty() ? INADDR_ANY : inet_addr(ip.c_str());
#endif
		int ret = bind(m_sock, (sockaddr*)&_sin, sizeof(sockaddr_in));
		if (SOCKET_ERROR == ret)
			std::cout << "Bind Error Port:" << port << std::endl;

		std::cout << "Bind <Sock:" << m_sock << ">"
			<< "IP: " << ip.c_str()
			<< " Port: " << port << std::endl;

		return ret;
	}

	int TCPServer::Listen(int n)
	{
		int ret = listen(m_sock, n);
		if (SOCKET_ERROR == ret)
			std::cout << "Listen Error <Sock" << m_sock << ">" << std::endl;

		std::cout << "Listen Done: <Sock:" << m_sock << ">" << std::endl;

		return ret;
	}

	bool TCPServer::Accept()
	{
		sockaddr_in clientAddr = {};
		int nAddrLen = sizeof(sockaddr_in);

		SOCKET clientSock = INVALID_SOCKET;
#ifdef _WIN32
		clientSock = accept(m_sock, (sockaddr*)&clientAddr, &nAddrLen);
#else
		clientSock = accept(m_sock, (sockaddr*)&clientAddr, (socklen_t*)(&nAddrLen));
#endif
		if (clientSock == INVALID_SOCKET)
		{
			std::cout << "Accept Error <Sock:" << m_sock << "> : invalid client" << std::endl;
			return false;
		}

		s2c_Join msg;
		msg.sock = clientSock;
		Send2All(&msg);

		m_clients.push_back(clientSock);
		std::cout << "<Sock:" << m_sock << "> New Client: Connection,  <Client Sock:" << clientSock << "> :IP = " << inet_ntoa(clientAddr.sin_addr) << std::endl;

		return true;
	}

	void TCPServer::CloseSock()
	{
		if (m_sock != INVALID_SOCKET)
		{

			for (SOCKET ele : m_clients)
			{
#ifdef _WIN32
				closesocket(ele);
#else
				close(ele);
#endif
			}

#ifdef _WIN32
			closesocket(m_sock);
#else
			close(m_sock);
#endif

#ifdef _WIN32
			WSACleanup();
#endif
		}
	}

	bool TCPServer::OnRun()
	{
		if (!IsRun())
			return false;

		fd_set fdRead;
		fd_set fdWrite;
		fd_set fdExp;

		FD_ZERO(&fdRead);
		FD_ZERO(&fdWrite);
		FD_ZERO(&fdExp);

		FD_SET(m_sock, &fdRead);
		FD_SET(m_sock, &fdWrite);
		FD_SET(m_sock, &fdExp);

		SOCKET maxSock = m_sock;

		for (int n = (int)m_clients.size() - 1; n >= 0; n--)
		{
			SOCKET clientSock = m_clients[n];
			FD_SET(clientSock, &fdRead);
			if (maxSock < clientSock)
				maxSock = clientSock;
		}

		timeval t = { 1,0 };
		int ret = select(maxSock + 1, &fdRead, &fdWrite, &fdExp, &t);
		if (ret < 0)
		{
			std::cout << "select over" << std::endl;
			CloseSock();
			return false;
		}

		if (FD_ISSET(m_sock, &fdRead))
		{
			FD_CLR(m_sock, &fdRead);

			Accept();
		}

		for (int n = (int)m_clients.size() - 1; n >= 0; n--)
		{
			if (FD_ISSET(m_clients[n], &fdRead))
			{
				if (-1 == Recv(m_clients[n]))
				{
					auto iter = m_clients.begin() + n;
					if (iter != m_clients.end())
					{
						m_clients.erase(iter);
					}
				}
			}
		}

		return true;
	}

	int TCPServer::Send(SOCKET cSock, DataHeader* msg)
	{
		if (IsRun() && msg)
		{
			return send(cSock, (const char*)msg, msg->dataLen, 0);
		}

		return SOCKET_ERROR;
	}

	void TCPServer::Send2All(DataHeader* msg)
	{
		for (int n = (int)m_clients.size() - 1; n >= 0; n--)
			Send(m_clients[n], msg);
	}

	int TCPServer::Recv(SOCKET cSock)
	{
		const int headerSize = sizeof(DataHeader);

		char szRecv[1024] = {};
		int nLenRecv = (int)recv(cSock, szRecv, headerSize, 0);
		DataHeader* header = (DataHeader*)szRecv;

		if (nLenRecv <= 0)
		{
			std::cout << "client <Socket=" << cSock << "> exit!" << std::endl;
			return -1;
		}

		recv(cSock, szRecv + headerSize, header->dataLen - headerSize, 0);
		OnMessageProc(cSock, header);

		return 0;
	}

	void TCPServer::OnMessageProc(SOCKET cSock, DataHeader* header)
	{
		switch (header->cmd)
		{
		case MessageType::MT_C2S_LOGIN:
		{
			c2s_Login* login = (c2s_Login*)header;

			std::cout << "recv " << "<Socket=" << cSock << "> cmd: " << (int)header->cmd << " len: " << login->dataLen << " username: " << login->userName << " password: " << login->passWord << std::endl;

			s2c_Login ret;
			ret.ret = 100;
			send(cSock, (char*)&ret, sizeof(s2c_Login), 0);
		}
		break;

		case MessageType::MT_C2S_LOGOUT:
		{
			c2s_Logout* logout = (c2s_Logout*)header;

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
	}

}; // end of namespace knet