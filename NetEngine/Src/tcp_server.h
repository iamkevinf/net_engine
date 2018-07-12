#ifndef __TCP_SERVERH_H__
#define __TCP_SERVERH_H__

#include "cell.h"
#include "net_time.h"
#include "net_event.h"

#include <atomic>

namespace knet
{
	class ClientSocket;
	class Cell;
	typedef std::vector<Cell*> CellPool;
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

		void Start();
		void AddClient2Cell(ClientSocket* client);

		int Send(SOCKET cSock, DataHeader* msg);

		void Time4Msg();

		void CloseSock();

		bool IsRun()const { return m_sock != INVALID_SOCKET; }
		bool OnRun();

		virtual void OnExit(ClientSocket* client) override;
		virtual void OnMessageProc(SOCKET cSock, DataHeader* header) override;

	private:
		SOCKET m_sock = INVALID_SOCKET;
		char m_buffer_recv[BUFFER_SIZE] = {0};

		CellPool m_cells;

		Time m_time;
		std::atomic_int m_recvCount = 0;
		std::atomic_int m_connCount = 0;
	};

}; // end of namespace knet

#endif // __TCP_SERVERH_H__