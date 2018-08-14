#include "server.h"

#include "client_socket.h"
#include "message.hpp"
#include "cell.h"

#include <iostream>
#include <string.h>

#include "Proto/player.pb.h"
#include "message.hpp"

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
	if (body->type == 100)
	{
		int size = body->size - knet::MessageBody::HEADER_TYPE_BYTES;

		CSLogin proto;
		if (proto.ParseFromArray(body->data, size))
		{
			std::cout << "Server::OnMessage type: " << body->type
				<< " size: " << body->size
				<< " username: " << proto.username()
				<< " password: " << proto.password()
				<< std::endl;

			SCLogin ret;
			ret.set_ret(true);
			int size = ret.ByteSize();

			knet::MessageBody s2cMsg;
			s2cMsg.size = size + knet::MessageBody::HEADER_TYPE_BYTES;
			s2cMsg.type = body->type + 1;

			ret.SerializeToArray(&s2cMsg.data, size);

			client->SendImm(&s2cMsg);
		}
	}
}

void Server::OnRecv(knet::ClientSocketPtr& client)
{
	knet::TCPServer::OnRecv(client);
	std::cout << "Server " << client->Sockfd() << " Recv" << std::endl;
}