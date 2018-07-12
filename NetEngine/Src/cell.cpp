﻿#include "cell.h"

#include <iostream>

#include "client_socket.h"
#include "message.hpp"
#include "net_event.h"

namespace knet
{

	Cell::Cell(SOCKET sock/*= INVALID_SOCKET*/, INetEvent* netEvent/*=nullptr*/) :m_sock(sock),
		m_netEvent(netEvent)
	{
	}

	Cell::~Cell()
	{
		CloseSock();
		m_sock = INVALID_SOCKET;

		if (m_thread)
		{
			delete m_thread;
			m_thread = nullptr;
		}
	}

	void Cell::Start()
	{
		m_thread = new std::thread(std::mem_fun(&Cell::OnRun), this);
	}

	bool Cell::IsRun()
	{
		return m_sock != INVALID_SOCKET;
	}

	void Cell::OnRun()
	{
		while (IsRun())
		{
			// lock block
			{
				std::lock_guard<std::mutex> lock(m_mutex);
				for (auto client : m_clientsBuff)
				{
					m_clients.push_back(client);
				}
				m_clientsBuff.clear();
			}

			if (m_clients.empty())
			{
				std::chrono::milliseconds t(1);
				std::this_thread::sleep_for(t);
				continue;
			}

			fd_set fdRead;
			FD_ZERO(&fdRead);
			// FD_SET(m_sock, &fdRead); // 主线程已经做了,这里不做FD_SET

			SOCKET maxSock = m_clients[0]->Sockfd();
			for (int n = (int)m_clients.size() - 1; n >= 0; n--)
			{
				SOCKET clientSock = m_clients[n]->Sockfd();
				FD_SET(clientSock, &fdRead);
				if (maxSock < clientSock)
					maxSock = clientSock;
			}

			int ret = select(maxSock + 1, &fdRead, nullptr, nullptr, nullptr);
			if (ret < 0)
			{
				std::cout << "select over" << std::endl;
				CloseSock();
				break;
			}

			for (int n = (int)m_clients.size() - 1; n >= 0; n--)
			{
				if (FD_ISSET(m_clients[n]->Sockfd(), &fdRead))
				{
					if (-1 == Recv(m_clients[n]))
					{
						auto iter = m_clients.begin() + n;
						if (iter != m_clients.end())
						{
							if (m_clients[n])
								m_netEvent->OnExit(m_clients[n]);

							delete m_clients[n];
							m_clients.erase(iter);
						}
					}
				}
			}
		}
	}


	int Cell::Recv(ClientSocket* clientSock)
	{
		const int headerSize = sizeof(DataHeader);

		int nLenRecv = (int)recv(clientSock->Sockfd(), m_buffer_recv, BUFFER_SIZE, 0);
		DataHeader* header = (DataHeader*)m_buffer_recv;

		if (nLenRecv <= 0)
		{
			std::cout << "client <Socket=" << clientSock->Sockfd() << "> exit!" << std::endl;
			return -1;
		}

		memcpy(clientSock->MsgBuffer() + clientSock->GetLastPos(), m_buffer_recv, nLenRecv);
		// m_buffer_msg尾巴的位置向后移动
		clientSock->SetLastPos(clientSock->GetLastPos() + nLenRecv);

		// 接收到的数据长度 >= 消息头的长度 就可以拿到消息头
		while (clientSock->GetLastPos() >= headerSize)
		{
			// 拿到消息头
			DataHeader* header = (DataHeader*)clientSock->MsgBuffer();

			// 接收到的数据长度 >= 消息本身的长度 说明一个消息已经收完
			if (clientSock->GetLastPos() >= header->dataLen)
			{
				// 剩余未处理的消息缓冲区的长度
				int nSize = clientSock->GetLastPos() - header->dataLen;

				OnMessageProc(clientSock->Sockfd(), header);

				// 消息缓冲区剩余未处理的数据前移
				memcpy(clientSock->MsgBuffer(), clientSock->MsgBuffer() + header->dataLen, nSize);
				// m_buffer_msg尾巴的位置向前移动
				clientSock->SetLastPos(nSize);
			}
			else // 说明没有收完一个消息,也就是剩余的不够一条消息
			{
				break;
			}
		}

		return 0;
	}


	void Cell::OnMessageProc(SOCKET cSock, DataHeader* header)
	{
		m_recvCount++;
		//double t = m_time.GetElapsedSecond();
		//if (t >= 1.0)
		//{
		//	::std::cout.setf(::std::ios::fixed);
		//	::std::cout << "<Socket = " << m_sock << ">"
		//		<< " CurrentTime = " << ::std::fixed << ::std::setprecision(6) << t
		//		<< " RecvCount = " << m_recvCount
		//		<< " ClientCount = " << m_clients.size() << ::std::endl;
		//	m_time.Update();
		//	m_recvCount = 0;
		//}
		switch (header->cmd)
		{
		case MessageType::MT_C2S_LOGIN:
		{
			c2s_Login* login = (c2s_Login*)header;

			//std::cout << "recv " << "<Socket=" << cSock << "> cmd: " << (int)header->cmd
			//	<< " len: " << login->dataLen
			//	<< " username: " << login->userName
			//	<< " password: " << login->passWord << std::endl;

			//s2c_Login ret;
			//strcpy(ret.userName, login->userName);
			//ret.ret = 100;
			//Send(cSock, &ret);
		}
		break;

		case MessageType::MT_C2S_LOGOUT:
		{
			c2s_Logout* logout = (c2s_Logout*)header;

			//std::cout << "recv " << "<Socket=" << cSock << "> cmd: " << (int)header->cmd
			//	<< " len: " << logout->dataLen
			//	<< " username: " << logout->userName << std::endl;

			//s2c_Logout ret;
			//ret.ret = 100;
			//Send(cSock, &ret);
		}
		break;

		default:
		{
			std::cout << "Undefined Msg" << "<Socket=" << cSock << "> cmd: " << (int)header->cmd
				<< " len: " << header->dataLen << std::endl;

			//header->cmd = MessageType::MT_ERROR;
			//Send(cSock, header);
		}
		break;
		}
	}

	void Cell::CloseSock()
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

	void Cell::AddClient(ClientSocket* client)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_clientsBuff.push_back(client);
	}

}; // end of namespace knet