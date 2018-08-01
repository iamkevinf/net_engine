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

void Server::OnMessage(knet::Cell* cell, knet::ClientSocketPtr& client, knet::DataHeader* header)
{
	knet::TCPServer::OnMessage(cell, client, header);
	std::cout << "Server " << client->Sockfd() << " OnMessage" << std::endl;

	switch (header->cmd)
	{
	case knet::MessageType::MT_C2S_HEART:
	{
		//knet::c2s_Heart* heart = (knet::c2s_Heart*)header;
		client->ResetHeart();

		knet::s2c_Heart* ret = new knet::s2c_Heart();
		//cell->AddSendTask(client, ret);
		if (SOCKET_ERROR == client->Send(ret))
		{
			// 消息缓冲区满了
			std::cout << "Msg Send Buffer Is Full <Socket=" << client->Sockfd() << ">" << std::endl;
		}
	}
	break;

	case knet::MessageType::MT_C2S_BODY:
	{
		knet::c2s_Body* body = (knet::c2s_Body*)header;
		int value = 0;
		memcpy(&value, body->data, body->nLen);

		std::cout << "s2c_Body " << " value: " << value << " nLen: " << body->nLen << std::endl;

		knet::s2c_Body* ret = new knet::s2c_Body();
		ret->nLen = body->nLen;
		value *= 2;
		memcpy(ret->data, (const char*)&value, ret->nLen);
		//cell->AddSendTask(client, ret);
		if (SOCKET_ERROR == client->Send(ret))
		{
			// 消息缓冲区满了
			std::cout << "Msg Send Buffer Is Full <Socket=" << client->Sockfd() << ">" << std::endl;
		}
	}
	break;

	default:
	{
		std::cout << "Undefined Msg" << "<Socket=" << client->Sockfd() << "> cmd: " << (int)header->cmd
			<< " len: " << header->dataLen << std::endl;

		header->cmd = knet::MessageType::MT_ERROR;
		//cell->AddSendTask(client, header);
		if (SOCKET_ERROR == client->Send(header))
		{
			// 消息缓冲区满了
			std::cout << "Msg Send Buffer Is Full <Socket=" << client->Sockfd() << ">" << std::endl;
		}
	}
	break;
	}
}

void Server::OnRecv(knet::ClientSocketPtr& client)
{
	knet::TCPServer::OnRecv(client);
	std::cout << "Server " << client->Sockfd() << " Recv" << std::endl;
}