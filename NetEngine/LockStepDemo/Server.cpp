#include "server.h"

#include "client_socket.h"
#include "message.hpp"
#include "cell.h"

#include <iostream>
#include <string.h>

Server::Server()
{

}

Server::~Server()
{
}

void Server::OnJoin(knet::ClientSocketPtr& client)
{
	knet::TCPServer::OnJoin(client);
	std::cout << "Server " << client->Sockfd() << " Join" << std::endl;
}

void Server::OnExit(knet::ClientSocketPtr& client)
{
	knet::TCPServer::OnExit(client);
	std::cout << "Server " << client->Sockfd() << " Exit" << std::endl;
}

void Server::OnMessage(knet::Cell* cell, knet::ClientSocketPtr& client, knet::MessageBody* body)
{
	int v = 0;
	::memcpy(&v, body->data, 4);
	std::cout << "Server::OnMessage type: " << body->type
		<< " size: " << body->size 
		<< " data: " << v
		<< std::endl;

	knet::MessageBody s2cMsg;
	int size = 4;
	s2cMsg.size = size + knet::MessageBody::HEADER_TYPE_BYTES;
	s2cMsg.type = body->type + 1;
	::memcpy(s2cMsg.data, body->data, size);
	client->Send(&s2cMsg);
}

void Server::OnRecv(knet::ClientSocketPtr& client)
{
	knet::TCPServer::OnRecv(client);
	std::cout << "Server " << client->Sockfd() << " Recv" << std::endl;
}