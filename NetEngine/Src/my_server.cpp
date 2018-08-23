#include "my_server.h"

#include "client_socket.h"
#include "message.hpp"
#include "cell.h"

#include <iostream>
#include <string.h>

MyServer::MyServer()
{

}

MyServer::~MyServer()
{
}

void MyServer::OnJoin(knet::ClientSocketPtr& client)
{
	knet::TCPServer::OnJoin(client);
	//LOG_TRACE("Client %d Join", client->Sockfd());
}

void MyServer::OnExit(knet::ClientSocketPtr& client)
{
	knet::TCPServer::OnExit(client);
	LOG_TRACE("Client %d Exit", client->Sockfd());
}

void MyServer::OnMessage(knet::Cell* cell, knet::ClientSocketPtr& client, knet::MessageBody* body)
{
	LOG_TRACE("MyServer::OnMessage type: %d size: %d", body->type, body->size);

	knet::MessageBody s2cMsg;
	int size = 4;
	int tmp = 66;
	s2cMsg.size = size + knet::MessageBody::HEADER_TYPE_BYTES;
	s2cMsg.type = body->type + 1;
	::memcpy(s2cMsg.data, &tmp, size);
	client->Send(&s2cMsg);
}

void MyServer::OnRecv(knet::ClientSocketPtr& client)
{
	knet::TCPServer::OnRecv(client);
	//LOG_TRACE("Client %d Recv", client->Sockfd());
}