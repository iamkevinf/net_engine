#ifndef __TCP_CELL_H__
#define __TCP_CELL_H__

#include "net_defined.hpp"

#include <string>
#include <vector>
#include <thread>
#include <mutex>

namespace knet
{
	class ClientSocket;
	struct DataHeader;
	class INetEvent;
	typedef std::vector<ClientSocket*> SockVector;

	class Cell
	{
	public:
		Cell(SOCKET sock=INVALID_SOCKET, INetEvent* netEvent=nullptr);
		virtual ~Cell();
		
		void Start();

		bool IsRun();
		void OnRun();

		//int Send(SOCKET sock, DataHeader* header);
		int Recv(ClientSocket* clientSock);

		void OnMessageProc(SOCKET cSock, DataHeader* header);

		void CloseSock();

		void AddClient(ClientSocket* client);

		size_t GetClientSize() const { return m_clients.size() + m_clientsBuff.size(); }

	private:
		SOCKET m_sock = INVALID_SOCKET;
		char m_buffer_recv[BUFFER_SIZE] = { 0 };

		SockVector m_clients;

		std::mutex m_mutex;
		SockVector m_clientsBuff;

		std::thread* m_thread = nullptr;
		INetEvent* m_netEvent = nullptr;
	};

}; // end of namespace knet

#endif // __TCP_CELL_H__