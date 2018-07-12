#ifndef __TCP_CELL_H__
#define __TCP_CELL_H__

#ifdef _WIN32
	#ifndef FD_SETSIZE
		#define FD_SETSIZE      4024
	#endif // FD_SETSIZE

	#define WIN32_LEAN_AND_MEAN
	#define _WINSOCK_DEPRECATED_NO_WARNINGS
	#define _CRT_SECURE_NO_WARNINGS
	
	#include <Windows.h>
	#include <WinSock2.h>
#else
	#include <unistd.h>
	#include <arpa/inet.h>
	
	#define SOCKET					int
	#define INVALID_SOCKET	(SOCKET)(~0)
	#define SOCKET_ERROR			(-1)
#endif

#include <vector>
#include <thread>
#include <atomic>
#include <mutex>

#include "net_defined.hpp"

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

		int Recv(ClientSocket* clientSock);

		void OnMessageProc(SOCKET cSock, DataHeader* header);

		void CloseSock();

		void AddClient(ClientSocket* client);

		size_t GetClientSize() const { return m_clients.size() + m_clientsBuff.size(); }

	private:
		SOCKET m_sock = INVALID_SOCKET;
		SockVector m_clients;
		SockVector m_clientsBuff;
		char m_buffer_recv[BUFFER_SIZE] = { 0 };

		std::mutex m_mutex;
		std::thread* m_thread = nullptr;
		INetEvent* m_netEvent = nullptr;

	public:
		std::atomic_int m_recvCount = 0;
	};

}; // end of namespace knet

#endif // __TCP_CELL_H__