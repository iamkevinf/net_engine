#ifndef __NET_MSG_TASK_H__
#define __NET_MSG_TASK_H__

#include "cell_task.h"
#include "client_socket.h"

#include <memory>

namespace knet
{
	class ClientSocket;
	struct DataHeader;

	typedef std::shared_ptr<class SendTask> SendTaskPtr;

	class SendTask : public CellTask
	{
	public:
		SendTask(ClientSocketPtr client, DataHeader* header);
		virtual ~SendTask();

		virtual void DoTask() override;

	private:
		ClientSocketPtr m_client;
		DataHeader* m_header;
	};

	/*******************************************************
	 * 
	 *******************************************************/
	class RecvTask : public CellTask
	{
	public:
		RecvTask();
		virtual ~RecvTask();

		virtual void DoTask() override;
	};


}; // end of namespace knet

#endif // __NET_MSG_TASK_H__