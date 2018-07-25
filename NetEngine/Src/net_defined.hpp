#ifndef __NET_DEFINED_HPP__
#define __NET_DEFINED_HPP__

#ifdef _WIN32
	#ifndef FD_SETSIZE
		#define FD_SETSIZE      2048
	#endif // FD_SETSIZE

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

#ifndef RECV_BUFFER_SIZE
#define RECV_BUFFER_SIZE 10240
#endif // RECV_BUFFER_SIZE

#ifndef SEND_BUFFER_SIZE
#define SEND_BUFFER_SIZE 10240
#endif // SEND_BUFFER_SIZE

#ifndef HEART_NT
// 客户端心跳时间间隔毫秒
#define HEART_NT 60000
#endif // HEART_NT

#endif // __NET_DEFINED_HPP__