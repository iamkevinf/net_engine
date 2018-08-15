#ifndef __WORLD_H__
#define __WORLD_H__

#include <map>
#include <string>
#include <queue>

#include "message.hpp"
#include "client_socket.h"

#include "Proto/player.pb.h"

#include "Player.h"

#define REGISTER_CMD_CALLBACK(cmdId, func) \
    m_command[uint16_t(cmdId)]  = std::bind(&World::func, this, std::placeholders::_1, std::placeholders::_2)

class World
{
	struct MsgNode
	{
		knet::ClientSocketPtr client;
		knet::MessageBody* msg;
	};

public:
	World();
	virtual ~World();

	typedef std::map<SOCKET, PlayerPtr> MapConnections;
	typedef std::function<bool(PlayerPtr&, knet::MessageBody*)> ServiceFunc;
	typedef std::queue<MsgNode> MsgQueue;

	void Start();
	void Tick();

	void PushMsg(const knet::ClientSocketPtr& client, knet::MessageBody* msg);
	void MessageProc();
	void DispatchCommand(MsgNode node);

	PlayerPtr GetUser(SOCKET id);
	bool RegisterUser(const knet::ClientSocketPtr& client);
	bool DeleteUser(const knet::ClientSocketPtr& client);

public:

	static World* GetInstance()
	{
		static World s_instance;
		return &s_instance;
	};

public:
	bool HandleLogin(PlayerPtr& player, knet::MessageBody* msg);

private:
	MapConnections m_sessions;
	MsgQueue m_msgQueue;

	std::map<uint16_t, ServiceFunc> m_command;

	uint64_t m_curFrameID = 0;
	uint64_t m_nxtFrameID = 0;

	uint32_t m_uuid_counter = 0;
};


#endif // __WORLD_H__