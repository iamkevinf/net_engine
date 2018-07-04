#include "client_socket.h"


namespace knet
{

	ClientSocket::ClientSocket(SOCKET sock/*=INVALID_SOCKET*/):m_sock(sock)
	{
		memset(m_buffer_msg, 0, sizeof(m_buffer_msg));
	}

	ClientSocket::~ClientSocket()
	{
	}

}; // end of namespace knet