#include "client_socket.h"
#include <string.h>
#include "message.hpp"

namespace knet
{

	ClientSocket::ClientSocket(SOCKET sock/*=INVALID_SOCKET*/):m_sock(sock)
	{
		memset(m_buffer_msg, 0, sizeof(m_buffer_msg));
	}

	ClientSocket::~ClientSocket()
	{
	}

	int ClientSocket::Send(DataHeader* header)
	{
		if (header)
		{
			return send(m_sock, (const char*)header, header->dataLen, 0);
		}

		return SOCKET_ERROR;
	}
}; // end of namespace knet