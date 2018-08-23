#include "tcp_server.h"
#include "message.hpp"
#include "client_socket.h"
#include "cell.h"

#include <iostream>
#include <iomanip>
#include <string.h>
#include <signal.h>

namespace knet
{

	TCPServer::TCPServer()
	{
	}

	TCPServer::~TCPServer()
	{
		Close();
	}

	void TCPServer::CreateSock()
	{
		if (m_sock != INVALID_SOCKET)
		{
			LOG_INFO("close old conn");
			Close();
		}

		m_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (m_sock == INVALID_SOCKET)
			LOG_ERROR("CreateSock Error");

		LOG_INFO("CreateSock Done <Sock:%d>", m_sock);
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
			LOG_ERROR("Bind Error Port: %d", port);

		LOG_INFO("Bind <Sock:%d> IP: %s Port: %d", m_sock, ip.c_str(), port);

		return ret;
	}

	int TCPServer::Listen(int n)
	{
		int ret = listen(m_sock, n);
		if (SOCKET_ERROR == ret)
			LOG_ERROR("Listen Error <Sock %d>", m_sock);

		LOG_INFO("Listen Done: <Sock: %d>", m_sock);

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
			LOG_ERROR("Accept Error <Sock: %d>: Invalid Client", m_sock);
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
	}

	void TCPServer::Start(int threadCount)
	{
		for (int i = 0; i < threadCount; ++i)
		{
			CellPtr cell = std::make_shared<Cell>(i);
			cell->SetNetEvent(this);
			m_cells.push_back(cell);

			cell->Start();
		}

		auto onRun = [this](CellThreadService* thread)
		{
			OnRun(thread);
		};

		m_threadService.Start(nullptr, onRun, nullptr);
	}

	void TCPServer::Close()
	{
		LOG_TRACE("TCPServer::Close");
		m_threadService.Close();

		if (m_sock != INVALID_SOCKET)
		{
			for (auto cell : m_cells)
			{
				cell.reset();
			}
			m_cells.clear();

#ifdef _WIN32
			closesocket(m_sock);
#else
			close(m_sock);
#endif

			m_sock = INVALID_SOCKET;
		}
	}

	void TCPServer::OnRun(CellThreadService* thread)
	{
		while (thread->IsRun())
		{

			Time4Msg();

			fd_set fdRead;
			FD_ZERO(&fdRead);
			FD_SET(m_sock, &fdRead);

			timeval t = { 0, 1 };
			int ret = select((int)(m_sock + 1), &fdRead, nullptr, nullptr, &t);
			if (ret < 0)
			{
				LOG_ERROR("TCPServer::OnRun Select Error");
				thread->CloseWithoutWait();
				break;
			}

			if (FD_ISSET(m_sock, &fdRead))
			{
				FD_CLR(m_sock, &fdRead);

				Accept();
			}

		}
	}

	void TCPServer::Time4Msg()
	{
		if (!m_logEnabled)
			return;

		double t = m_time.GetElapsedSecond();
		if (t >= 1.0)
		{
			LOG_INFO("Thread=%d Socket=%d Time=%f Conn=%d Rec=%d Msg=%d", m_cells.size(), m_sock, t, m_connCount.load(), m_recvCount.load(), int(m_msgCount.load() / t));

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

	void TCPServer::OnMessage(Cell* cell, ClientSocketPtr& client, MessageBody* header)
	{
		m_msgCount++;
	}

	void TCPServer::OnRecv(ClientSocketPtr& client)
	{
		m_recvCount++;
	}

}; // end of namespace knet
