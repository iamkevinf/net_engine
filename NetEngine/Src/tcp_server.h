#ifndef __TCP_SERVERH_H__
#define __TCP_SERVERH_H__

#include "net_defined.hpp"
#include "net_time.h"
#include "net_event.h"

#include <atomic>
#include <vector>

namespace knet
{
	class ClientSocket;
	typedef std::shared_ptr<class ClientSocket> ClientSocketPtr;

	class Cell;
	typedef std::shared_ptr<class Cell> CellPtr;
	typedef std::vector<CellPtr> CellPool;

	struct DataHeader;

	class TCPServer : INetEvent
	{
	public:
		TCPServer();
		virtual ~TCPServer();

		void CreateSock();

		int Bind(const std::string& ip, uint16_t port);
		int Listen(int n);
		bool Accept();

		void Start(int threadCount);
		void AddClient2Cell(ClientSocketPtr client);

		void Time4Msg();

		void CloseSock();

		bool IsRun()const { return m_sock != INVALID_SOCKET; }
		bool OnRun();

		virtual void OnJoin(ClientSocketPtr& client) override;
		virtual void OnExit(ClientSocketPtr& client) override;
		virtual void OnMessage(Cell* cell, ClientSocketPtr& client, DataHeader* header) override;
		virtual void OnRecv(ClientSocketPtr& client) override;

	protected:
		std::atomic<int> m_msgCount{0};
		std::atomic<int> m_connCount{0};
		std::atomic<int> m_recvCount{0};

	private:
		SOCKET m_sock = INVALID_SOCKET;
		char m_buffer_recv[RECV_BUFFER_SIZE] = {0};

		CellPool m_cells;

		Time m_time;

	};

}; // end of namespace knet

#endif // __TCP_SERVERH_H__