#ifndef __NET_EVENT_H__
#define __NET_EVENT_H__

#include "net_defined.hpp"
#include "client_socket.h"

namespace knet
{
	struct DataHeader;
	class Cell;

	class INetEvent
	{
	public:
		virtual void OnJoin(ClientSocketPtr& client) = 0;
		virtual void OnExit(ClientSocketPtr& client) = 0;
		virtual void OnMessage(Cell* cell, ClientSocketPtr& client, DataHeader* header) = 0;
		virtual void OnRecv(ClientSocketPtr& client) = 0;
	};

}; // end of namespace knet

#endif // __NET_EVENT_H__