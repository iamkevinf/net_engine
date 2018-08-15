#include "server.h"

#include "client_socket.h"
#include "message.hpp"
#include "cell.h"

#include <iostream>
#include <string.h>

#include "Proto/player.pb.h"
#include "Proto/message_type.pb.h"
#include "message.hpp"

#include "World.h"

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
	World::GetInstance()->RegisterUser(client);
}

void Server::OnExit(knet::ClientSocketPtr& client)
{
	knet::TCPServer::OnExit(client);
	std::cout << "Server " << client->Sockfd() << " Exit" << std::endl;
	World::GetInstance()->DeleteUser(client);
}

void Server::OnMessage(knet::Cell* cell, knet::ClientSocketPtr& client, knet::MessageBody* body)
{
	World::GetInstance()->PushMsg(client, body);
}

void Server::OnRecv(knet::ClientSocketPtr& client)
{
	knet::TCPServer::OnRecv(client);
	std::cout << "Server " << client->Sockfd() << " Recv" << std::endl;
}