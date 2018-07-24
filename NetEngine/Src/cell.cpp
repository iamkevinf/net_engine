#include "cell.h"

#include <iostream>
#include <functional> // std::mem_fn
#include <string.h> // memcpy

#include "message.hpp"
#include "net_event.h"

#include "net_msg_task.h"

namespace knet
{

	Cell::Cell(SOCKET sock/*= INVALID_SOCKET*/) :m_sock(sock)
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
		m_thread = new std::thread(std::mem_fn(&Cell::OnRun), this);
		m_taskService.Start();
	}

	bool Cell::IsRun()
	{
		return m_sock != INVALID_SOCKET;
	}

	void Cell::OnRun()
	{
		m_connDelta = true;

		while (IsRun())
		{
			// lock block
			if(m_clientsBuff.size() > 0)
			{
				std::lock_guard<std::mutex> lock(m_mutex);
				for (auto client : m_clientsBuff)
				{
					m_clients[client->Sockfd()] = client;
				}
				m_clientsBuff.clear();
				m_connDelta = true;
			}

			if (m_clients.empty())
			{
				std::chrono::milliseconds t(1);
				std::this_thread::sleep_for(t);
				continue;
			}

			fd_set fdRead;
			FD_ZERO(&fdRead);

			if (m_connDelta)
			{
				m_connDelta = false;
				m_maxSock = m_clients.begin()->second->Sockfd();
				for(auto iter : m_clients)
				{
					FD_SET(iter.second->Sockfd(), &fdRead);
					if (m_maxSock < iter.second->Sockfd())
						m_maxSock = iter.second->Sockfd();
				}

				memcpy(&m_fdReadBak, &fdRead, sizeof(fd_set));
			}
			else
			{
				memcpy(&fdRead, &m_fdReadBak, sizeof(fd_set));
			}

			timeval t = {0,0};
			int ret = select(m_maxSock + 1, &fdRead, nullptr, nullptr, &t);
			if (ret < 0)
			{
				std::cout << "select over" << std::endl;
				CloseSock();
				break;
			}
			else if (ret == 0)
			{
				continue;
			}

#ifdef _WIN32
			for (u_int i = 0; i < fdRead.fd_count; ++i)
			{
				auto iter = m_clients.find(fdRead.fd_array[i]);
				if (iter != m_clients.end())
				{
					if (-1 == Recv(iter->second))
					{
						if (m_netEvent)
							m_netEvent->OnExit(iter->second);

						m_connDelta = true;

						m_clients.erase(iter->first);
					}
				}
				else
				{
					std::cout << "OnRun:: m_clients.find Error" << std::endl;
				}
			}
#else
			SockVector temp;
			for (auto iter : m_clients)
			{
				if (FD_ISSET(iter.second->Sockfd(), &fdRead))
				{
					if (-1 == Recv(iter.second))
					{
						if (m_netEvent)
							m_netEvent->OnExit(iter.second);

						m_connDelta = true;
						temp.push_back(iter.second.get());
					}
				}
			}
			for (auto client : temp)
			{
				m_clients.erase(client->Sockfd());
				delete client;
			}
#endif
		}
	}

	int Cell::Recv(ClientSocketPtr& clientSock)
	{
		const int headerSize = sizeof(DataHeader);

		int nLenRecv = (int)recv(clientSock->Sockfd(), m_buffer_recv, BUFFER_SIZE, 0);
		m_netEvent->OnRecv(clientSock);

		DataHeader* header = (DataHeader*)m_buffer_recv;

		if (nLenRecv <= 0)
		{
			//std::cout << "client <Socket=" << clientSock->Sockfd() << "> exit!" << std::endl;
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

				OnMessageProc(clientSock, header);

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


	void Cell::OnMessageProc(ClientSocketPtr& client, DataHeader* header)
	{
		m_netEvent->OnMessage(this, client, header);
	}

	void Cell::CloseSock()
	{
		if (m_sock != INVALID_SOCKET)
		{

			for(auto iter : m_clients)
			{
#ifdef _WIN32
				closesocket(iter.second->Sockfd());
#else
				close(iter.second->Sockfd());
#endif
		}

			m_clients.clear();

#ifdef _WIN32
			closesocket(m_sock);
#else
			close(m_sock);
#endif

	}
}

	void Cell::AddClient(ClientSocketPtr& client)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_clientsBuff.push_back(client);
	}

	void Cell::AddSendTask(ClientSocketPtr& client, DataHeader* header)
	{
		SendTaskPtr task = std::make_shared<SendTask>(client, header);
		m_taskService.AddTask(task);
	}

}; // end of namespace knet