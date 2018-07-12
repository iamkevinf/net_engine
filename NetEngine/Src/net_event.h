#ifndef __NET_EVENT_H__
#define __NET_EVENT_H__

#include "net_defined.hpp"

namespace knet
{
	class ClientSocket;
	struct DataHeader;

	class INetEvent
	{
	public:
		virtual void OnExit(ClientSocket* client) = 0;
		virtual void OnMessageProc(SOCKET cSock, DataHeader* header) = 0;
	};

}; // end of namespace knet

#endif // __NET_EVENT_H__