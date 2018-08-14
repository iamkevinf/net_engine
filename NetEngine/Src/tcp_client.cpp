﻿#include "tcp_client.h"
#include "message.hpp"

#include <iostream>
#include <string.h>

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

		//std::cout << "CreateSock Done <Sock:" << m_sock << ">" << std::endl;
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
			std::cout << "Conn Error <Socket: " << m_sock << ">" << std::endl;
		else
			m_isConn = true;

		//std::cout << "Conn <Socket:" << m_sock 
		//	<< "> IP: " << ip.c_str()
		//	<< " Port: " << port << std::endl;

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

			std::cout << "CloseSock <Socket:" << m_sock << ">" << std::endl;

			m_sock = INVALID_SOCKET;
		}
		m_isConn = false;
	}

	bool TCPClient::OnRun()
	{
		if (!IsRun())
			return false;

		fd_set fdReads;
		FD_ZERO(&fdReads);
		FD_SET(m_sock, &fdReads);

		timeval tv = { 0,0 };
		int ret = select((int)(m_sock + 1), &fdReads, 0, 0, &tv);
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


	int TCPClient::Send(MessageBody* body)
	{
		int32_t size = body->GetSize();
		const char* buf = (const char*)body;

		int ret = SOCKET_ERROR;
		if (IsRun() && body)
		{
			ret = send(m_sock, buf, size, 0);
			if (ret == SOCKET_ERROR)
				CloseSock();
		}

		return ret;
	}

	int TCPClient::GetPackageLength()
	{
		if (m_lastPos < MessageBody::HEADER_SIZE)
			return -1;

		int len = -1;
		::memcpy(&len, m_buffer_msg, MessageBody::HEADER_LEN_BYTES);
		if (m_lastPos >= len + MessageBody::HEADER_LEN_BYTES)
			return len;

		return -1;
	}

	int TCPClient::Recv()
	{
		int nLen = (int)recv(m_sock, m_buffer_recv, RECV_BUFFER_SIZE, 0);

		if (nLen <= 0)
			return -1;

		while (true)
		{
			if (m_lastPos < MessageBody::HEADER_SIZE)
				break;

			int len = GetPackageLength();
			if (len < 0)
				break;

			MessageBody body;
			size_t copysize = sizeof(MessageBody) <= m_lastPos ? sizeof(MessageBody) : m_lastPos;

			::memcpy(&body, m_buffer_recv, copysize);

			int body_size = len + MessageBody::HEADER_LEN_BYTES;
			if (body_size > m_lastPos)
				body_size = m_lastPos;

			::memmove(m_buffer_recv, m_buffer_recv + body_size, body_size);
			m_lastPos = m_lastPos - body_size;

			OnMessageProc(&body);

			bool isFinish = false;
			if (m_lastPos <= 0)
				isFinish = true;

			if (isFinish)
				break;
		}

		return nLen;
	}

	void TCPClient::OnMessageProc(MessageBody* body)
	{
		std::cout << "TCPClient::OnMessageProc type: " << body->type
			<< " size: " << body->size
			<< std::endl;
	}

}; // end of namespace knet