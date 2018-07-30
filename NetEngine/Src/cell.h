#ifndef __TCP_CELL_H__
#define __TCP_CELL_H__

#include "net_defined.hpp"

#include "cell_task.h"

#include <string>
#include <vector>
#include <map>
#include <thread>
#include <mutex>
#include <memory>

#include "client_socket.h"
#include "semaphore.h"

namespace knet
{
	struct DataHeader;
	class INetEvent;

	typedef std::shared_ptr<class Cell> CellPtr;
	typedef std::vector<CellPtr> CellPool;
	typedef std::vector<ClientSocketPtr> SockPtrVector;
	typedef std::map<SOCKET, ClientSocketPtr> Sock2ClientPool;

	class Cell
	{
	public:
		Cell(int id);
		virtual ~Cell();
		
		void SetNetEvent(INetEvent* netEvent) { m_netEvent = netEvent; }

		void Start();

		bool IsRun();
		void OnRun();

		int Recv(ClientSocketPtr& clientSock);

		void OnMessageProc(ClientSocketPtr& client, DataHeader* header);

		void Close();

		void AddClient(ClientSocketPtr& client);

		size_t GetClientSize() const { return m_clients.size() + m_clientsBuff.size(); }

		void AddSendTask(ClientSocketPtr& client, DataHeader* header);

		const int GetID()const { return m_id; }

	private:
		void ReadData(fd_set& fdRead);
		void CheckTime();

		void ClrClient();

	private:
		char m_buffer_recv[RECV_BUFFER_SIZE] = { 0 };

		Sock2ClientPool m_clients;

		std::mutex m_mutex;
		SockPtrVector m_clientsBuff;

		std::thread* m_thread = nullptr;
		INetEvent* m_netEvent = nullptr;

		fd_set m_fdReadBak = {0};
		SOCKET m_maxSock = INVALID_SOCKET;

		CellTaskService m_taskService;

		time_t m_last_heart = -1;

		int m_id = -1;

		Semaphore m_semaphore;

		bool m_connDelta = true;
		bool m_isRun = false;
	};

}; // end of namespace knet

#endif // __TCP_CELL_H__