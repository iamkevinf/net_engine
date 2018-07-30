#include "cell.h"

#include <iostream>
#include <functional> // std::mem_fn
#include <string.h> // memcpy

#include "message.hpp"
#include "net_event.h"
#include "net_time.h"

namespace knet
{

	Cell::Cell(int id):m_id(id)
	{
		m_taskService.SetOwner(this);
	}

	Cell::~Cell()
	{
		Close();

		if (m_thread)
		{
			delete m_thread;
			m_thread = nullptr;
		}
	}

	void Cell::Start()
	{
		if (m_isRun)
			return;

		m_isRun = true;

		m_thread = new std::thread(std::mem_fn(&Cell::OnRun), this);
		m_thread->detach();

		m_taskService.Start();
	}

	bool Cell::IsRun()
	{
		return m_isRun;
	}

	void Cell::OnRun()
	{
		m_connDelta = true;

		while (IsRun())
		{
			// lock block
			if (m_clientsBuff.size() > 0)
			{
				std::lock_guard<std::mutex> lock(m_mutex);
				for (auto client : m_clientsBuff)
				{
					if (m_netEvent)
						m_netEvent->OnJoin(client);

					m_clients[client->Sockfd()] = client;
				}
				m_clientsBuff.clear();
				m_connDelta = true;
			}

			if (m_clients.empty())
			{
				std::chrono::milliseconds t(1);
				std::this_thread::sleep_for(t);
				m_last_heart = Time::GetCurTime();
				continue;
			}

			fd_set fdRead;
			FD_ZERO(&fdRead);

			if (m_connDelta)
			{
				m_connDelta = false;
				m_maxSock = m_clients.begin()->second->Sockfd();
				for (auto iter : m_clients)
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

			timeval t = { 0,1 };
			int ret = select((int)(m_maxSock + 1), &fdRead, nullptr, nullptr, &t);
			if (ret < 0)
			{
				std::cout << "select over" << std::endl;
				Close();
				break;
			}
			//else if (ret == 0)
			//{
			//	continue;
			//}

			ReadData(fdRead);
			CheckTime();
		}

		std::cout << "Cell::OnRun exit " << m_id << std::endl;
		ClrClient();
		m_semaphore.Weakup();
	}

	void Cell::CheckTime()
	{
		if (m_last_heart < 0)
			m_last_heart = Time::GetCurTime();

		time_t tt = Time::GetCurTime();
		time_t dTime = tt - m_last_heart;
		m_last_heart = tt;
		for (auto iter = m_clients.begin(); iter != m_clients.end(); )
		{
			if (iter->second->CheckHeart(dTime))
			{
				if (m_netEvent)
					m_netEvent->OnExit(iter->second);

				m_connDelta = true;

				iter->second.reset();

				m_clients.erase(iter++);
				continue;
			}

			iter->second->CheckSend(dTime);

			iter++;
		}
	}

	void Cell::ReadData(fd_set& fdRead)
	{
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

					iter->second.reset();

					m_clients.erase(iter);
				}
			}
			else
			{
				std::cout << "OnRun:: m_clients.find Error" << std::endl;
			}
		}
#else
		SockPtrVector temp;
		for (auto iter : m_clients)
		{
			if (FD_ISSET(iter.second->Sockfd(), &fdRead))
			{
				if (-1 == Recv(iter.second))
				{
					if (m_netEvent)
						m_netEvent->OnExit(iter.second);

					m_connDelta = true;

					iter->second.reset();

					temp.push_back(iter.second);
				}
			}
		}
		for (auto client : temp)
		{
			m_clients.erase(client->Sockfd());
			//client.reset();
		}
#endif
	}

	int Cell::Recv(ClientSocketPtr& clientSock)
	{
		const int headerSize = sizeof(DataHeader);

		//接收客户端数据
		char* szRecv = clientSock->MsgBuffer() + clientSock->GetLastPos();
		int nLen = (int)recv(clientSock->Sockfd(), szRecv, (RECV_BUFFER_SIZE)-clientSock->GetLastPos(), 0);
		m_netEvent->OnRecv(clientSock);
		if (nLen <= 0)
		{
			//std::cout << "client <Socket=" << clientSock->Sockfd() << "> exit!" << std::endl;
			return -1;
		}

		clientSock->ResetHeart();

		////将收取到的数据拷贝到消息缓冲区
		//memcpy(clientSock->MsgBuffer() + clientSock->GetLastPos(), m_buffer_recv, nLen);
		//消息缓冲区的数据尾部位置后移
		clientSock->SetLastPos(clientSock->GetLastPos() + nLen);

		//判断消息缓冲区的数据长度大于消息头DataHeader长度
		while (clientSock->GetLastPos() >= headerSize)
		{
			//这时就可以知道当前消息的长度
			DataHeader* header = (DataHeader*)clientSock->MsgBuffer();
			//判断消息缓冲区的数据长度大于消息长度
			if (clientSock->GetLastPos() >= header->dataLen)
			{
				//消息缓冲区剩余未处理数据的长度
				int nSize = clientSock->GetLastPos() - header->dataLen;
				//处理网络消息
				OnMessageProc(clientSock, header);
				//将消息缓冲区剩余未处理数据前移
				memcpy(clientSock->MsgBuffer(), clientSock->MsgBuffer() + header->dataLen, nSize);
				//消息缓冲区的数据尾部位置前移
				clientSock->SetLastPos(nSize);
			}
			else
			{
				//消息缓冲区剩余数据不够一条完整消息
				break;
			}
		}
		return 0;
	}


	void Cell::OnMessageProc(ClientSocketPtr& client, DataHeader* header)
	{
		m_netEvent->OnMessage(this, client, header);
	}

	void Cell::Close()
	{
		if (!m_isRun)
			return;

		std::cout << "Cell::Close " << m_id << std::endl;

		m_taskService.Close();
		m_isRun = false;

		m_semaphore.Wait();
	}

	void Cell::ClrClient()
	{
		for (auto iter : m_clients)
		{
			iter.second.reset();
		}
		m_clients.clear();

		for (auto client : m_clientsBuff)
		{
			client.reset();
		}
		m_clientsBuff.clear();
	}

	void Cell::AddClient(ClientSocketPtr& client)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_clientsBuff.push_back(client);
	}

	void Cell::AddSendTask(ClientSocketPtr& client, DataHeader* header)
	{
		m_taskService.AddTask([client, header]()
		{
			client->Send(header);
			delete header;
		});
	}

}; // end of namespace knet