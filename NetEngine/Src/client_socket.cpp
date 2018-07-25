#include "client_socket.h"
#include <string.h>
#include "message.hpp"

namespace knet
{

	ClientSocket::ClientSocket(SOCKET sock/*=INVALID_SOCKET*/):m_sock(sock)
	{
		memset(m_buffer_msg, 0, RECV_BUFFER_SIZE);
		memset(m_send_buffer, 0, SEND_BUFFER_SIZE);
	}

	ClientSocket::~ClientSocket()
	{
	}

	int ClientSocket::Send(DataHeader* header)
	{
		int ret = SOCKET_ERROR;
		//要发送的数据长度
		int nSendLen = header->dataLen;
		//要发送的数据
		const char* pSendData = (const char*)header;

		while (true)
		{
			if (m_lastSendPos + nSendLen >= SEND_BUFFER_SIZE)
			{
				//计算可拷贝的数据长度
				int nCopyLen = SEND_BUFFER_SIZE - m_lastSendPos;
				//拷贝数据
				memcpy(m_send_buffer + m_lastSendPos, pSendData, nCopyLen);
				//计算剩余数据位置
				pSendData += nCopyLen;
				//计算剩余数据长度
				nSendLen -= nCopyLen;
				//发送数据
				ret = send(m_sock, m_send_buffer, SEND_BUFFER_SIZE, 0);
				//数据尾部位置清零
				m_lastSendPos = 0;
				//发送错误
				if (SOCKET_ERROR == ret)
					return ret;
			}
			else
			{
				//将要发送的数据 拷贝到发送缓冲区尾部
				memcpy(m_send_buffer + m_lastSendPos, pSendData, nSendLen);
				//计算数据尾部位置
				m_lastSendPos += nSendLen;
				break;
			}
		}
		return ret;
	}

	bool ClientSocket::CheckHeart(time_t dtime)
	{
		m_heart += dtime;
		return m_heart >= HEART_NT;
	}

}; // end of namespace knet