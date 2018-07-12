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

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 10240
#endif // BUFFER_SIZE

#ifndef MSG_BUFFER_SIZE
#define MSG_BUFFER_SIZE (BUFFER_SIZE*10)
#endif // MSG_BUFFER_SIZE

#ifndef CELL_THREAD_COUNT
#define CELL_THREAD_COUNT 8
#endif // CELL_THREAD_COUNT

#endif // __NET_DEFINED_HPP__