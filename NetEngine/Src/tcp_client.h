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
	struct MessageBody;

	class TCPClient
	{
	public:
		TCPClient();
		virtual ~TCPClient();

		void CreateSock();

		int Conn(const std::string& ip, uint16_t port);

		int Send(MessageBody* body);

		int Recv();

		int GetPackageLength();

		void CloseSock();

		bool OnRun();
		bool IsRun()const { return m_sock != INVALID_SOCKET && m_isConn; }

		void OnMessageProc(MessageBody* header);

	private:
		SOCKET m_sock = INVALID_SOCKET;
		char m_buffer_recv[RECV_BUFFER_SIZE] = {};
		char m_buffer_msg[RECV_BUFFER_SIZE * 10] = {};
		// m_buffer_msg尾巴的位置
		int m_lastPos = 0;


		bool m_isConn = false;
	};

}; // end of namespace knet

#endif // __TCP_CLIENT_H__