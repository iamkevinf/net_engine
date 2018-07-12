#ifndef __NET_EVENT_H__
#define __NET_EVENT_H__

namespace knet
{
	class ClientSocket;

	class INetEvent
	{
	public:
		virtual void OnExit(ClientSocket* client) = 0;
	};

}; // end of namespace knet

#endif // __NET_EVENT_H__