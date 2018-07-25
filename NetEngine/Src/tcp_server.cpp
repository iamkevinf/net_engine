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

		ClientSocketPtr client(new ClientSocket(clientSock));
		AddClient2Cell(client);

		return true;
	}

	void TCPServer::AddClient2Cell(ClientSocketPtr client)
	{
		CellPtr minCell = m_cells[0];
		for (auto cell : m_cells)
		{
			if (minCell->GetClientSize() > cell->GetClientSize())
				minCell = cell;
		}

		minCell->AddClient(client);
		OnJoin(client);
	}

	void TCPServer::Start(int threadCount)
	{
		for (int i = 0; i < threadCount; ++i)
		{
			CellPtr cell = std::make_shared<Cell>(m_sock);
			cell->SetNetEvent(this);
			m_cells.push_back(cell);

			cell->Start();
		}
	}

	void TCPServer::CloseSock()
	{
		if (m_sock != INVALID_SOCKET)
		{
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
		FD_ZERO(&fdRead);
		FD_SET(m_sock, &fdRead);

		timeval t = { 0,10 };
		int ret = select((int)(m_sock + 1), &fdRead, nullptr, nullptr, &t);
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

	void TCPServer::Time4Msg()
	{
		double t = m_time.GetElapsedSecond();
		if (t >= 1.0)
		{
			::std::cout.setf(::std::ios::fixed);
			::std::cout << "Thread=" << m_cells.size()
				<< " Socket " << m_sock
				<< " Time=" << ::std::fixed << ::std::setprecision(6) << t
				<< " Conn=" << m_connCount
				<< " Rec=" << m_recvCount
				<< " Msg=" << int(m_msgCount / t)
				<< ::std::endl;

			m_msgCount = 0;
			m_recvCount = 0;
			m_time.Update();
		}
	}

	void TCPServer::OnJoin(ClientSocketPtr& client)
	{
		m_connCount++;
	}

	void TCPServer::OnExit(ClientSocketPtr& client)
	{
		m_connCount--;
	}

	void TCPServer::OnMessage(Cell* cell, ClientSocketPtr& client, DataHeader* header)
	{
		m_msgCount++;
	}

	void TCPServer::OnRecv(ClientSocketPtr& client)
	{
		m_recvCount++;
	}

}; // end of namespace knet