#ifndef __TCP_SERVERH_H__
#define __TCP_SERVERH_H__

#ifdef _WIN32
	#define WIN32_LEAN_AND_MEAN
	#define _WINSOCK_DEPRECATED_NO_WARNINGS

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
#include <string>

namespace knet
{
	typedef std::vector<SOCKET> SockVector;
	struct DataHeader;

	class TCPServer
	{
	public:
		TCPServer();
		virtual ~TCPServer();

		void CreateSock();

		int Bind(const std::string& ip, uint16_t port);
		int Listen(int n);
		bool Accept();

		int Send(SOCKET cSock, DataHeader* msg);
		void Send2All(DataHeader* msg);
		int Recv(SOCKET cSock);

		void CloseSock();

		bool IsRun()const { return m_sock != INVALID_SOCKET; }
		bool OnRun();

		void OnMessageProc(SOCKET cSock, DataHeader* header);

	private:
		SOCKET m_sock = INVALID_SOCKET;
		SockVector m_clients;
	};

}; // end of namespace knet

#endif // __TCP_SERVERH_H__