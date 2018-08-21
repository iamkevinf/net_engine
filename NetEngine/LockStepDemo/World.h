#ifndef __WORLD_H__
#define __WORLD_H__

#include <map>
#include <vector>
#include <string>
#include <queue>

#include "message.hpp"
#include "client_socket.h"

#include "Proto/player.pb.h"

#include "Player.h"

#define REGISTER_CMD_CALLBACK(cmdId, func) \
    m_command[uint16_t(cmdId)]  = std::bind(&World::func, this, std::placeholders::_1, std::placeholders::_2)

enum class EGameState
{
	EGS_Ready = 1,
	EGS_RUNNING = 2,
	EGS_OVER = 3
};

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
	typedef std::map<uint32_t, std::vector<uint32_t>> UID2KeyPool;

	void Start();
	void Tick();

	void PushMsg(const knet::ClientSocketPtr& client, knet::MessageBody* msg);
	void MessageProc();
	void DispatchCommand(MsgNode node);

	PlayerPtr GetUser(SOCKET id);
	uint32_t GetUserSize()const { return m_sessions.size(); }
	bool RegisterUser(const knet::ClientSocketPtr& client);
	bool DeleteUser(const knet::ClientSocketPtr& client);

	void Broadcast(knet::MessageBody* msg);
	void Broadcast_but(knet::MessageBody* msg, const PlayerPtr& client);

	void FrameInit(const PlayerPtr& player);
	void FrameTurn();

public:

	static World* GetInstance()
	{
		static World s_instance;
		return &s_instance;
	};

public:
	bool HandleLogin(PlayerPtr& player, knet::MessageBody* msg);
	bool HandleReady(PlayerPtr& player, knet::MessageBody* msg);
	bool HandleFrame(PlayerPtr& player, knet::MessageBody* msg);

private:
	MapConnections m_sessions;
	MsgQueue m_msgQueue;
	EGameState m_gameState = EGameState::EGS_Ready;
	uint32_t m_readyCount = 0;

	std::map<uint16_t, ServiceFunc> m_command;

	uint64_t m_curFrameID = 0;
	uint64_t m_nxtFrameID = 0;

	uint32_t m_uuid_counter = 0;

	UID2KeyPool m_curFrameInfo;
	std::map<uint64_t, UID2KeyPool> m_oldFrameInfo;
};


#endif // __WORLD_H__