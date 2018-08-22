#include "client_socket.h"
#include <string.h>
#include <iostream>
#include "message.hpp"

namespace knet
{

	ClientSocket::ClientSocket(SOCKET sock/*=INVALID_SOCKET*/) :m_sock(sock)
	{
		memset(m_buffer_msg, 0, RECV_BUFFER_SIZE);
		memset(m_send_buffer, 0, SEND_BUFFER_SIZE);
	}

	ClientSocket::~ClientSocket()
	{
		LOG_TRACE("ClientSocket::~ClientSocket");

		if (INVALID_SOCKET != m_sock)
		{

#ifdef _WIN32
			closesocket(m_sock);
#else
			close(m_sock);
#endif // _WIN32

			m_sock = INVALID_SOCKET;
		}
	}

	int ClientSocket::GetPackageLength()
	{
		if (GetLastPos() < MessageBody::HEADER_SIZE)
			return -1;

		int len = -1;
		::memcpy(&len, m_buffer_msg, MessageBody::HEADER_LEN_BYTES);
		if (GetLastPos() >= len + MessageBody::HEADER_LEN_BYTES)
			return len;

		return -1;
	}

	int ClientSocket::Send(MessageBody* header)
	{
		int ret = SOCKET_ERROR;
		//要发送的数据长度
		int nSendLen = header->size + MessageBody::HEADER_LEN_BYTES;
		//要发送的数据
		const char* pSendData = (const char*)header;

		if (m_lastSendPos + nSendLen <= SEND_BUFFER_SIZE)
		{
			//将要发送的数据 拷贝到发送缓冲区尾部
			memcpy(m_send_buffer + m_lastSendPos, pSendData, nSendLen);
			//计算数据尾部位置
			m_lastSendPos += nSendLen;

			if (m_lastSendPos == SEND_BUFFER_SIZE)
			{
				m_sendBuffFullCount++;
			}

			return nSendLen;
		}

		m_sendBuffFullCount++;

		return ret;
	}

	int ClientSocket::SendImm()
	{
		int ret = 0;
		if (m_lastSendPos > 0 && INVALID_SOCKET != m_sock)
		{
			//发送数据
			ret = send(m_sock, m_send_buffer, m_lastSendPos, 0);
			//数据尾部位置清零
			m_lastSendPos = 0;
			//
			m_sendBuffFullCount = 0;
			//
			ResetLastSend();
		}

		return ret;
	}

	void ClientSocket::SendImm(MessageBody* header)
	{
		Send(header);
		SendImm();
	}

	bool ClientSocket::CheckHeart(time_t dtime)
	{
		m_heart += dtime;
		return m_heart >= HEART_NT;
	}

	bool ClientSocket::CheckSend(time_t dtime)
	{
		m_lastSendDT += dtime;
		bool ret = m_lastSendDT >= SEND_BUFF_NT;
		if (ret)
		{
			SendImm();
			ResetLastSend();
		}

		return ret;
	}
}; // end of namespace knet