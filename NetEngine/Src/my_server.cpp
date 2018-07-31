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
	//std::cout << "Client " << client->Sockfd() << " Join" << std::endl;
}

void MyServer::OnExit(knet::ClientSocketPtr& client)
{
	knet::TCPServer::OnExit(client);
	std::cout << "Client " << client->Sockfd() << " Exit" << std::endl;
}

void MyServer::OnMessage(knet::Cell* cell, knet::ClientSocketPtr& client, knet::DataHeader* header)
{
	knet::TCPServer::OnMessage(cell, client, header);

	switch (header->cmd)
	{
	case knet::MessageType::MT_C2S_LOGIN:
	{
		knet::c2s_Login* login = (knet::c2s_Login*)header;

		//std::cout << "recv " << "<Socket=" << client->Sockfd() << "> cmd: " << (int)header->cmd
		//	<< " len: " << login->dataLen
		//	<< " username: " << login->userName
		//	<< " password: " << login->passWord << std::endl;

		knet::s2c_Login* ret = new knet::s2c_Login();
		strcpy(ret->userName, login->userName);
		ret->ret = 100;

		//cell->AddSendTask(client, ret);
		if (SOCKET_ERROR == client->Send(ret))
		{
			// 消息缓冲区满了
			std::cout << "Msg Send Buffer Is Full <Socket=" << client->Sockfd() << ">" << std::endl;
		}
	}
	break;

	case knet::MessageType::MT_C2S_LOGOUT:
	{
		knet::c2s_Logout* logout = (knet::c2s_Logout*)header;

		//std::cout << "recv " << "<Socket=" << client->Sockfd() << "> cmd: " << (int)header->cmd
		//	<< " len: " << logout->dataLen
		//	<< " username: " << logout->userName << std::endl;

		knet::s2c_Logout* ret = new knet::s2c_Logout();
		ret->ret = 100;
		//cell->AddSendTask(client, ret);
		if (SOCKET_ERROR == client->Send(ret))
		{
			// 消息缓冲区满了
			std::cout << "Msg Send Buffer Is Full <Socket=" << client->Sockfd() << ">" << std::endl;
		}
	}
	break;

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

void MyServer::OnRecv(knet::ClientSocketPtr& client)
{
	knet::TCPServer::OnRecv(client);
	//std::cout << "Client " << client->Sockfd() << " Recv" << std::endl;
}