#ifndef __TCP_CLIENT_H__
#define __TCP_CLIENT_H__

#ifdef _WIN32
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
#endif // _WIN32

#include <string>
#include "net_defined.hpp"

namespace knet
{
	struct DataHeader;

	class TCPClient
	{
	public:
		TCPClient();
		virtual ~TCPClient();

		void CreateSock();

		int Conn(const std::string& ip, uint16_t port);

		int Send(DataHeader* msg, int nLen);

		int Recv();

		void CloseSock();

		bool OnRun();
		bool IsRun()const { return m_sock != INVALID_SOCKET; }

		void OnMessageProc(DataHeader* header);

	private:
		SOCKET m_sock = INVALID_SOCKET;
		char m_buffer_recv[BUFFER_SIZE] = {};
		char m_buffer_msg[MSG_BUFFER_SIZE] = {};
		// m_buffer_msgÎ²°ÍµÄÎ»ÖÃ
		int m_lastPos = 0;
	};

}; // end of namespace knet

#endif // __TCP_CLIENT_H__