#include "tcp_client.h"
#include "message.hpp"

#include <iostream>

namespace knet
{

	TCPClient::TCPClient()
	{
	}

	TCPClient::~TCPClient()
	{
		CloseSock();
	}

	void TCPClient::CreateSock()
	{
#ifdef _WIN32
		WORD ver = MAKEWORD(2, 2);
		WSADATA data;
		WSAStartup(ver, &data);
#endif
		if (m_sock != INVALID_SOCKET)
		{
			std::cout << "close old conn" << std::endl;
			CloseSock();
		}

		m_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (m_sock == INVALID_SOCKET)
			std::cout << "CreateSock Error" << std::endl;

		std::cout << "CreateSock Done <Sock:" << m_sock << ">" << std::endl;
	}

	int TCPClient::Conn(const std::string& ip, uint16_t port)
	{
		if (m_sock == INVALID_SOCKET)
			CreateSock();

		sockaddr_in sin = {};
		sin.sin_family = AF_INET;
		sin.sin_port = htons(port);
#ifdef _WIN32
		sin.sin_addr.S_un.S_addr = inet_addr(ip.c_str());
#else
		sin.sin_addr.s_addr = inet_addr(ip.c_str());
#endif

		int ret = connect(m_sock, (sockaddr*)&sin, sizeof(sockaddr_in));
		if (SOCKET_ERROR == ret)
			std::cout << "Conn Error" << std::endl;

		std::cout << "Conn <Socket:" << m_sock 
			<< "> IP: " << ip.c_str()
			<< " Port: " << port << std::endl;

		return ret;
	}

	void TCPClient::CloseSock()
	{
		if (m_sock != INVALID_SOCKET)
		{
#ifdef _WIN32
			closesocket(m_sock);
#else
			close(m_sock);
#endif

			std::cout << "CloseSock <Socket:" << m_sock << std::endl;

			m_sock = INVALID_SOCKET;
		}
	}

	bool TCPClient::OnRun()
	{
		if (!IsRun())
			return false;

		fd_set fdReads;
		FD_ZERO(&fdReads);
		FD_SET(m_sock, &fdReads);

		timeval tv = { 1,0 };
		int ret = select(m_sock + 1, &fdReads, 0, 0, &tv);
		if (ret < 0)
		{
			std::cout << "Select Error: <socket=" << m_sock << ">" << std::endl;
			CloseSock();
			return false;
		}

		if (FD_ISSET(m_sock, &fdReads))
		{
			FD_CLR(m_sock, &fdReads);

			if (-1 == Recv())
			{
				std::cout << "Recv Error: <socket=" << m_sock << ">" << std::endl;
				CloseSock();
				return false;
			}
		}

		return true;
	}

	int TCPClient::Send(DataHeader* msg)
	{
		if (IsRun() && msg)
		{
			return send(m_sock, (const char*)msg, msg->dataLen, 0);
		}

		return SOCKET_ERROR;
	}

	int TCPClient::Recv()
	{
		const int headerSize = sizeof(DataHeader);

		int nLenRecv = (int)recv(m_sock, m_buffer_recv, BUFFER_SIZE, 0);
		if (nLenRecv <= 0)
		{
			std::cout << "disconnection from server" << std::endl;
			return -1;
		}

		memcpy(m_buffer_msg + m_lastPos, m_buffer_recv, nLenRecv);
		// m_buffer_msg尾巴的位置向后移动
		m_lastPos += nLenRecv;

		// 接收到的数据长度 > 消息头的长度 就可以拿到消息头
		while (m_lastPos >= headerSize)
		{
			// 拿到消息头
			DataHeader* header = (DataHeader*)m_buffer_msg;

			// 接收到的数据长度 > 消息本身的长度 说明一个消息已经收完
			if (m_lastPos >= header->dataLen)
			{
				// 剩余未处理的消息缓冲区的长度
				int nSize = m_lastPos - header->dataLen;

				OnMessageProc(header);
				
				// 消息缓冲区剩余未处理的数据前移
				memcpy(m_buffer_msg, m_buffer_msg + header->dataLen, nSize);
				// m_buffer_msg尾巴的位置向前移动
				m_lastPos = nSize;
			}
			else // 说明没有收完一个消息,也就是剩余的不够一条消息
			{
				break;
			}
		}

		return 0;
	}

	void TCPClient::OnMessageProc(DataHeader* header)
	{
		switch (header->cmd)
		{
		case MessageType::MT_S2C_LOGIN:
		{
			s2c_Login* ret = (s2c_Login*)header;
			//std::cout << "s2c_Login " << "<Socket = " << m_sock 
			//	<< "> userName: " << ret->userName
			//	<< " ret: " << ret->ret
			//	<< " dataLen: " << ret->dataLen << std::endl;
		}
		break;

		case MessageType::MT_S2C_LOGOUT:
		{
			s2c_Logout* ret = (s2c_Logout*)header;
			//std::cout << "s2c_Logout " << "<Socket=" << m_sock << "> ret: "
			//	<< ret->ret << " dataLen: " << ret->dataLen << std::endl;

		}
		break;

		case MessageType::MT_S2C_JOIN:
		{
			s2c_Join* ret = (s2c_Join*)header;
			//std::cout << "s2c_Join " << "<Socket = " << m_sock  << "> sock: "
			//	<< ret->sock << " dataLen: " << ret->dataLen << std::endl;
		}
		break;

		case MessageType::MT_ERROR:
		{
			std::cout << "Error Message " << "<Socket = " << m_sock << ">" << std::endl;
		}
		break;

		default:
		{
			std::cout << "Undefined Message: " << "<Socket=" << m_sock << "> dataLen: "
				<< header->dataLen << std::endl;
		}
		break;

		}
	}

}; // end of namespace knet