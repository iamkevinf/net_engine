#include "tcp_server.h"
#include "message.hpp"
#include "client_socket.h"
#include "cell.h"

#include <iostream>
#include <iomanip>
#include <string.h>

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

		//s2c_Join msg;
		//msg.sock = clientSock;
		//Send2All(&msg);

		AddClient2Cell(new ClientSocket(clientSock));

		//std::cout << "<Sock:" << m_sock << "> New Client<" << m_clients.size() << ">: Connection <Client Sock:" << clientSock << "> :IP = " << inet_ntoa(clientAddr.sin_addr) << std::endl;

		return true;
	}

	void TCPServer::AddClient2Cell(ClientSocket* client)
	{
		m_clients.push_back(client);

		Cell* minCell = m_cells[0];
		for (auto cell : m_cells)
		{
			if (minCell->GetClientSize() > cell->GetClientSize())
				minCell = cell;
		}

		minCell->AddClient(client);
	}

	void TCPServer::Start()
	{
		for (int i = 0; i < CELL_THREAD_COUNT; ++i)
		{
			auto cell = new Cell(m_sock, this);
			m_cells.push_back(cell);

			cell->Start();
		}
	}

	void TCPServer::CloseSock()
	{
		if (m_sock != INVALID_SOCKET)
		{

			for (int n = (int)m_clients.size() - 1; n >= 0; n--)
			{
				ClientSocket* ele = m_clients[n];
#ifdef _WIN32
				closesocket(ele->Sockfd());
#else
				close(ele->Sockfd());
#endif
				delete ele;
			}

			m_clients.clear();

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

		Time4Msg();

		fd_set fdRead;
		//fd_set fdWrite;
		//fd_set fdExp;

		FD_ZERO(&fdRead);
		//FD_ZERO(&fdWrite);
		//FD_ZERO(&fdExp);

		FD_SET(m_sock, &fdRead);
		//FD_SET(m_sock, &fdWrite);
		//FD_SET(m_sock, &fdExp);

		timeval t = { 0,10 };
		int ret = select(m_sock + 1, &fdRead, nullptr/*&fdWrite*/, nullptr/*&fdExp*/, &t);
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

			return true;
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
			Send(m_clients[n]->Sockfd(), msg);
	}

	void TCPServer::Time4Msg()
	{
		double t = m_time.GetElapsedSecond();
		if (t >= 1.0)
		{
			int recvCount = 0;
			for (auto cell : m_cells)
			{
				recvCount += cell->m_recvCount;
				cell->m_recvCount = 0;
			}

			::std::cout.setf(::std::ios::fixed);
			::std::cout << "ThreadCount=" << m_cells.size()
				<< " <Socket=" << m_sock << ">"
				<< " Time=" << ::std::fixed << ::std::setprecision(6) << t
				<< " ClientCount=" << m_clients.size()
				<< " RecvCount=" << recvCount//int(recvCount / t)
				<< ::std::endl;

			m_time.Update();
		}
	}

	void TCPServer::OnExit(ClientSocket* client)
	{
		for (int i = (int)m_clients.size() - 1; i >= 0; i--)
		{
			if (m_clients[i] == client)
			{
				auto iter = m_clients.begin() + i;
				if(iter != m_clients.end())
					m_clients.erase(iter);
			}
		}
	}

}; // end of namespace knet