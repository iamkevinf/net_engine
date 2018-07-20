#ifndef __CLIENT_SOCKET_H__
#define __CLIENT_SOCKET_H__

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
#endif


#include "net_defined.hpp"
#include <memory>

namespace knet
{
	struct DataHeader;

	typedef std::shared_ptr<class ClientSocket> ClientSocketPtr;

	class ClientSocket
	{
	public:
		ClientSocket(SOCKET sock = INVALID_SOCKET);
		virtual ~ClientSocket();

		SOCKET Sockfd() const { return m_sock; }
		char* MsgBuffer() { return m_buffer_msg; }

		int GetLastPos() const { return m_lastPos; }
		void SetLastPos(int pos) { m_lastPos = pos; }

		int Send(DataHeader* header);

	private:
		SOCKET m_sock = INVALID_SOCKET;

		char m_buffer_msg[MSG_BUFFER_SIZE] = { 0 };
		// m_buffer_msgÎ²°ÍµÄÎ»ÖÃ
		int m_lastPos = 0;
	};

}; // end of namespace knet

#endif // __CLIENT_SOCKET_H__