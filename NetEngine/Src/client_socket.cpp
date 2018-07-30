#include "client_socket.h"
#include <string.h>
#include <iostream>
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
		std::cout << "ClientSocket::~ClientSocket " << m_sock << std::endl;

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
				// 
				ResetLastSend();
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

	int ClientSocket::SendImm()
	{
		int ret = SOCKET_ERROR;
		if (m_lastSendPos > 0 && SOCKET_ERROR != m_sock)
		{
			//发送数据
			ret = send(m_sock, m_send_buffer, m_lastSendPos, 0);
			//数据尾部位置清零
			m_lastSendPos = 0;
			//
			ResetLastSend();
		}

		return ret;
	}

	void ClientSocket::SendImm(DataHeader* header)
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