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

namespace knet
{
	struct DataHeader;
	class INetEvent;

	typedef std::shared_ptr<class Cell> CellPtr;
	typedef std::vector<CellPtr> CellPool;
	typedef std::vector<ClientSocketPtr> SockVector;
	typedef std::map<SOCKET, ClientSocketPtr> Sock2ClientPool;

	class Cell
	{
	public:
		Cell(SOCKET sock=INVALID_SOCKET);
		virtual ~Cell();
		
		void SetNetEvent(INetEvent* netEvent) { m_netEvent = netEvent; }

		void Start();

		bool IsRun();
		void OnRun();

		int Recv(ClientSocketPtr& clientSock);

		void OnMessageProc(ClientSocketPtr& client, DataHeader* header);

		void CloseSock();

		void AddClient(ClientSocketPtr& client);

		size_t GetClientSize() const { return m_clients.size() + m_clientsBuff.size(); }

		void AddSendTask(ClientSocketPtr& client, DataHeader* header);


	private:
		SOCKET m_sock = INVALID_SOCKET;
		char m_buffer_recv[BUFFER_SIZE] = { 0 };

		Sock2ClientPool m_clients;

		std::mutex m_mutex;
		SockVector m_clientsBuff;

		std::thread* m_thread = nullptr;
		INetEvent* m_netEvent = nullptr;

		fd_set m_fdReadBak = {0};
		bool m_connDelta = true;
		SOCKET m_maxSock = INVALID_SOCKET;

		CellTaskService m_taskService;
	};

}; // end of namespace knet

#endif // __TCP_CELL_H__