#ifndef __NET_MSG_TASK_H__
#define __NET_MSG_TASK_H__

#include "cell_task.h"

namespace knet
{
	class ClientSocket;
	struct DataHeader;

	class SendTask : public CellTask
	{
	public:
		SendTask(ClientSocket* client, DataHeader* header);
		virtual ~SendTask();

		virtual void DoTask() override;

	private:
		ClientSocket* m_client;
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