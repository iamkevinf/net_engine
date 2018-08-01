#ifndef _SERVER_H__
#define _SERVER_H__

#include "tcp_server.h"

class Server : public knet::TCPServer
{
public:
	Server();
	virtual ~Server();

	virtual void OnJoin(knet::ClientSocketPtr& client) override;
	virtual void OnExit(knet::ClientSocketPtr& client) override;
	virtual void OnMessage(knet::Cell* cell, knet::ClientSocketPtr& client, knet::DataHeader* header) override;
	virtual void OnRecv(knet::ClientSocketPtr& client) override;

};


#endif // _MY_SERVER_H__