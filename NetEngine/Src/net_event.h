#ifndef __NET_EVENT_H__
#define __NET_EVENT_H__

#include "net_defined.hpp"
#include "client_socket.h"

namespace knet
{
	struct MessageBody;
	class Cell;

	class INetEvent
	{
	public:
		virtual void OnJoin(ClientSocketPtr& client) = 0;
		virtual void OnExit(ClientSocketPtr& client) = 0;
		virtual void OnMessage(Cell* cell, ClientSocketPtr& client, MessageBody* header) = 0;
		virtual void OnRecv(ClientSocketPtr& client) = 0;
	};

}; // end of namespace knet

#endif // __NET_EVENT_H__