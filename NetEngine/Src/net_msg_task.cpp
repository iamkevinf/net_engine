#include "net_msg_task.h"

#include "client_socket.h"
#include "message.hpp"

namespace knet
{

	SendTask::SendTask(ClientSocket* client, DataHeader* header):m_client(client)
		,m_header(header)
	{

	}

	SendTask::~SendTask()
	{

	}

	void SendTask::DoTask()
	{
		m_client->Send(m_header);
		delete m_header;
		m_header = nullptr;
	}

	/*******************************************************
	*
	*******************************************************/

}; // end of namespace knet