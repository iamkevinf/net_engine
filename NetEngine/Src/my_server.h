#ifndef _MY_SERVER_H__
#define _MY_SERVER_H__

#include "tcp_server.h"

class MyServer : public knet::TCPServer
{
public:
	MyServer();
	virtual ~MyServer();

	virtual void OnJoin(knet::ClientSocket* client) override;
	virtual void OnExit(knet::ClientSocket* client) override;
	virtual void OnMessage(knet::Cell* cell, knet::ClientSocket* client, knet::DataHeader* header) override;
	virtual void OnRecv(knet::ClientSocket* client) override;

};


#endif // _MY_SERVER_H__