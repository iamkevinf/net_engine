#ifndef __CLIENT_SOCKET_H__
#define __CLIENT_SOCKET_H__

#include "net_defined.hpp"
#include <memory>

#include "object_pool.h"

namespace knet
{
	struct DataHeader;

	typedef std::shared_ptr<class ClientSocket> ClientSocketPtr;

	class ClientSocket : public ObjectPoolBase<ClientSocket, 10000>
	{
	public:
		ClientSocket(SOCKET sock = INVALID_SOCKET);
		virtual ~ClientSocket();

		SOCKET Sockfd() const { return m_sock; }
		char* MsgBuffer() { return m_buffer_msg; }

		int GetLastPos() const { return m_lastPos; }
		void SetLastPos(int pos) { m_lastPos = pos; }

		int Send(DataHeader* header);
		int SendImm();
		void SendImm(DataHeader* header);

		void ResetHeart() { m_heart = 0; };
		void ResetLastSend() { m_lastSendDT = 0; }

		bool CheckHeart(time_t dtime);
		bool CheckSend(time_t dtine);

	private:
		SOCKET m_sock = INVALID_SOCKET;

		char m_buffer_msg[RECV_BUFFER_SIZE];
		char m_send_buffer[SEND_BUFFER_SIZE];
		// m_buffer_msg尾巴的位置
		int m_lastPos = 0;
		// 发送缓冲区的数据尾部位置
		int m_lastSendPos = 0;
		// 死亡计时
		time_t m_heart = 0;
		time_t m_lastSendDT = 0;
	};

}; // end of namespace knet

#endif // __CLIENT_SOCKET_H__