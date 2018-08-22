#include "cell.h"

#include <iostream>
#include <functional> // std::mem_fn
#include <string.h> // memcpy

#include "message.hpp"
#include "net_event.h"
#include "net_time.h"

namespace knet
{

	Cell::Cell(int id) :m_id(id)
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
		m_taskService.Start();

		auto onRun = [this](CellThreadService* thread)
		{
			OnRun(thread);
		};

		auto onDestroy = [this](CellThreadService* thread)
		{
			ClrClient();
		};

		m_threadService.Start(nullptr, onRun, onDestroy);
	}


	void Cell::OnRun(CellThreadService* thread)
	{
		m_connDelta = true;

		while (thread->IsRun())
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
			fd_set fdWrite;
			//fd_set fdExcp;

			if (m_connDelta)
			{
				m_connDelta = false;

				FD_ZERO(&fdRead);
				FD_ZERO(&fdWrite);
				//FD_ZERO(&fdExcp);

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

			memcpy(&fdWrite, &m_fdReadBak, sizeof(fd_set));
			//memcpy(&fdExcp, &m_fdReadBak, sizeof(fd_set));

			timeval t = { 0,1 };
			int ret = select((int)(m_maxSock + 1), &fdRead, &fdWrite, nullptr, &t);
			if (ret < 0)
			{
				LOG_TRACE("Cell::OnRun:: Select Error");
				thread->CloseWithoutWait();
				break;
			}
			//else if (ret == 0)
			//{
			//	continue;
			//}

			ReadData(fdRead);
			//WriteData(fdWrite);
			//WriteData(fdExcp);

#if _WIN32
			//LOG_INFO("Cell::OnRun Select: fdRead=%d fdWrite=%d", fdRead.fd_count, fdWrite.fd_count);

			//if (fdExcp.fd_count > 0)
			//{
			//	LOG_INFO("Cell::OnRun::Select::Excp=%d",fdExcp.fd_count);
			//}
#endif

			//CheckTime();
		}

		LOG_TRACE("Cell::OnRun exit");
		ClrClient();
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

			//iter->second->CheckSend(dTime);

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
					OnClientExit(iter->second);
					m_clients.erase(iter);
				}
			}
			else
			{
				LOG_ERROR("OnRun:: m_clients.find Error");
			}
		}
#else
		for (auto iter = m_clients.begin(); iter != m_clients.end();)
		{
			if (FD_ISSET(iter->second->Sockfd(), &fdRead))
			{
				if (-1 == Recv(iter->second))
				{
					OnClientExit(iter->second);
					m_clients.erase(iter++);
					continue;
				}
			}
			iter++;
		}
#endif
	}

	void Cell::WriteData(fd_set& fdWrite)
	{
#ifdef _WIN32
		for (u_int i = 0; i < fdWrite.fd_count; ++i)
		{
			auto iter = m_clients.find(fdWrite.fd_array[i]);
			if (iter != m_clients.end())
			{
				if (-1 == iter->second->SendImm())
				{
					OnClientExit(iter->second);
					m_clients.erase(iter);
				}
			}
		}
#else
		for (auto iter = m_clients.begin(); iter != m_clients.end();)
		{
			if (FD_ISSET(iter->second->Sockfd(), &fdWrite))
			{
				if (-1 == iter->second->SendImm())
				{
					OnClientExit(iter->second);
					m_clients.erase(iter++);
					continue;
				}
			}
			iter++;
		}
#endif
	}

	void Cell::OnClientExit(ClientSocketPtr client)
	{
		if (m_netEvent)
			m_netEvent->OnExit(client);

		m_connDelta = true;

		client.reset();
	}

	int Cell::Recv(ClientSocketPtr& clientSock)
	{
		char* szRecv = clientSock->MsgBuffer() + clientSock->GetLastPos();
		int nLen = (int)recv(clientSock->Sockfd(), szRecv, (RECV_BUFFER_SIZE)-clientSock->GetLastPos(), 0);
		clientSock->SetLastPos(clientSock->GetLastPos() + nLen);

		if (nLen <= 0)
			return -1;

		while (true)
		{
			if (clientSock->GetLastPos() < MessageBody::HEADER_SIZE)
				break;

			int len = clientSock->GetPackageLength();
			if (len < 0)
				break;

			MessageBody body;
			size_t copysize = sizeof(MessageBody) <= clientSock->GetLastPos() ? sizeof(MessageBody) : clientSock->GetLastPos();

			::memcpy(&body, clientSock->MsgBuffer(), copysize);

			int body_size = len + MessageBody::HEADER_LEN_BYTES;
			if (body_size > clientSock->GetLastPos())
				body_size = clientSock->GetLastPos();

			::memmove(clientSock->MsgBuffer(), clientSock->MsgBuffer() + body_size, body_size);
			clientSock->SetLastPos(clientSock->GetLastPos() - body_size);

			OnMessageProc(clientSock, &body);

			bool isFinish = false;
			if (clientSock->GetLastPos() <= 0)
				isFinish = true;

			if (isFinish)
				break;
		}

		return nLen;
	}

	void Cell::OnMessageProc(ClientSocketPtr& client, MessageBody* body)
	{
		m_netEvent->OnMessage(this, client, body);
	}

	void Cell::Close()
	{
		m_taskService.Close();
		m_threadService.Close();
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

	void Cell::AddSendTask(ClientSocketPtr& client, MessageBody* header)
	{
		m_taskService.AddTask([client, header]()
		{
			client->Send(header);
			delete header;
		});
	}

}; // end of namespace knet
