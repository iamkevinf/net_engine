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
	LOG_INFO("Server %d Join", client->Sockfd());
	World::GetInstance()->RegisterUser(client);
}

void Server::OnExit(knet::ClientSocketPtr& client)
{
	knet::TCPServer::OnExit(client);
	LOG_INFO("Server %d Exit", client->Sockfd());
	World::GetInstance()->DeleteUser(client);
}

void Server::OnMessage(knet::Cell* cell, knet::ClientSocketPtr& client, knet::MessageBody* body)
{
	World::GetInstance()->PushMsg(client, body);
}

void Server::OnRecv(knet::ClientSocketPtr& client)
{
	knet::TCPServer::OnRecv(client);
	LOG_INFO("Server %d Recv", client->Sockfd());
}