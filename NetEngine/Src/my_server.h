#ifndef _MY_SERVER_H__
#define _MY_SERVER_H__

#include "tcp_server.h"

class MyServer : public knet::TCPServer
{
public:
	MyServer();
	virtual ~MyServer();

	virtual void OnJoin(knet::ClientSocketPtr& client) override;
	virtual void OnExit(knet::ClientSocketPtr& client) override;
	virtual void OnMessage(knet::Cell* cell, knet::ClientSocketPtr& client, knet::MessageBody* body) override;
	virtual void OnRecv(knet::ClientSocketPtr& client) override;

};


#endif // _MY_SERVER_H__