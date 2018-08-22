#include "World.h"

#include "Proto/message_type.pb.h"

World::World()
{
	REGISTER_CMD_CALLBACK(EMessageType::ECSLogin, HandleLogin);
	REGISTER_CMD_CALLBACK(EMessageType::ECSReady, HandleReady);
	REGISTER_CMD_CALLBACK(EMessageType::ECSFrame, HandleFrame);
}

World::~World()
{
}


void World::Start()
{
	Tick();
}

void World::Tick()
{
	MessageProc();

	auto curTime = m_timer.GetElapsedTimeMS();
	if (curTime > 100)
		Turn();
}

void World::Turn()
{
	m_timer.Update();
	if (m_gameState == EGameState::EGS_RUNNING)
	{
		FrameTurn();
	}
}

void World::PushMsg(const knet::ClientSocketPtr& client, knet::MessageBody* msg)
{
	MsgNode node{ client, msg };
	m_msgQueue.push(node);
}

void World::MessageProc()
{
	if (m_msgQueue.size() > 0)
	{
		MsgNode node = m_msgQueue.front();
		DispatchCommand(node);
		m_msgQueue.pop();
	}
}

PlayerPtr World::GetUser(SOCKET id)
{
	if (m_sessions.find(id) != m_sessions.end())
		return m_sessions[id];

	return PlayerPtr();
}

void World::DispatchCommand(MsgNode node)
{
	knet::ClientSocketPtr client = node.client;
	knet::MessageBody* msg = node.msg;
	if (m_command.find(msg->type) != m_command.end())
	{
		PlayerPtr player = GetUser(client->Sockfd());
		if (player)
		{
			m_command[msg->type](player, msg);
			return;
		}
	}
	else
	{

	}
}

bool World::RegisterUser(const knet::ClientSocketPtr& client)
{
	assert(m_sessions.find(client->Sockfd()) == m_sessions.end());
	m_sessions.insert(std::make_pair(client->Sockfd(), PlayerPtr(new Player(client))));
	return true;
}

bool World::DeleteUser(const knet::ClientSocketPtr& client)
{
	assert(m_sessions.find(client->Sockfd()) != m_sessions.end());
	m_sessions.erase(client->Sockfd());
	return true;
}

void World::Broadcast(knet::MessageBody* msg)
{
	for (auto iter = m_sessions.begin(); iter != m_sessions.end(); ++iter)
	{
		iter->second->SendImm(msg);
	}
}

void World::Broadcast_but(knet::MessageBody* msg, const PlayerPtr& player)
{
	for (auto iter = m_sessions.begin(); iter != m_sessions.end(); ++iter)
	{
		if (iter->second->GetUUID() == player->GetUUID())
			continue;

		iter->second->SendImm(msg);
	}
}

bool World::HandleLogin(PlayerPtr& player, knet::MessageBody* msg)
{
	if (!msg)
		return false;

	int size = msg->size - knet::MessageBody::HEADER_TYPE_BYTES;

	CSLogin proto;
	if (!proto.ParseFromArray(msg->data, size))
		return false;

	if (player->GetUUID() != 0)
	{
		std::cout << "This player alread exist " << player->GetUUID() << std::endl;
		return false;
	}

	m_uuid_counter++;
	player->SetUUID(m_uuid_counter);

	std::cout << "HandleLogin cur uuid: " << player->GetUUID() << std::endl;

	SCLogin ret;
	ret.set_uuid(player->GetUUID());
	size = ret.ByteSize();

	knet::MessageBody s2cMsg;
	s2cMsg.size = size + knet::MessageBody::HEADER_TYPE_BYTES;
	s2cMsg.type = EMessageType::ESCLogin;

	ret.SerializeToArray(&s2cMsg.data, size);

	player->SendImm(&s2cMsg);

	FrameInit(player);
	SendLoginInfo();

	return true;
}

bool World::HandleReady(PlayerPtr& player, knet::MessageBody* msg)
{
	if (!msg)
		return false;

	int size = msg->size - knet::MessageBody::HEADER_TYPE_BYTES;

	CSReady proto;
	if (!proto.ParseFromArray(msg->data, size))
		return false;

	if (player->GetUUID() != 0)
	{
		m_readyCount++;
		if (m_readyCount == GetUserSize())
			m_gameState = EGameState::EGS_RUNNING;
	}

	std::cout << "HandleReady " << player->GetUUID() << std::endl;

	return true;
}

bool World::HandleFrame(PlayerPtr& player, knet::MessageBody* msg)
{
	if (!msg)
		return false;

	int size = msg->size - knet::MessageBody::HEADER_TYPE_BYTES;

	CSFrame proto;
	if (!proto.ParseFromArray(msg->data, size))
		return false;

	uint32_t uuid = proto.uuid();
	uint64_t frameID = proto.frameid();

	for (int i = 0; i < proto.keyinfo_size(); ++i)
	{
		uint32_t key = proto.keyinfo()[i];
		m_curFrameInfo.insert(std::make_pair(uuid, std::vector<uint32_t>())).first->second.push_back(key);
	}

	return true;
}

void World::SendLoginInfo()
{
	SCSight proto;
	PlayerInfo* playerInfo = nullptr;

	for (auto iter : m_sessions)
	{
		playerInfo = proto.add_players();
		playerInfo->set_uuid(iter.second->GetUUID());
		playerInfo->set_posx(iter.second->GetPosX());
		playerInfo->set_posy(iter.second->GetPosY());
		playerInfo->set_posz(iter.second->GetPosZ());
	}

	int size = proto.ByteSize();

	knet::MessageBody s2cMsg;
	s2cMsg.size = size + knet::MessageBody::HEADER_TYPE_BYTES;
	s2cMsg.type = EMessageType::ESCSight;

	proto.SerializeToArray(&s2cMsg.data, size);
	Broadcast(&s2cMsg);
}

void World::FrameInit(const PlayerPtr& player)
{
	SCFrameInit frame;

	frame.set_curframeid(m_curFrameID);
	frame.set_nxtframeid(m_nxtFrameID);

	int size = frame.ByteSize();

	knet::MessageBody s2cMsg;
	s2cMsg.size = size + knet::MessageBody::HEADER_TYPE_BYTES;
	s2cMsg.type = EMessageType::ESCFrameInit;

	frame.SerializeToArray(&s2cMsg.data, size);
	player->SendImm(&s2cMsg);
}

void World::FrameTurn()
{
	SCFrame sFrame;
	UserFrame* uFrame;

	m_curFrameID = m_nxtFrameID;
	m_nxtFrameID++;

	sFrame.set_frameid(m_curFrameID);
	sFrame.set_nextframeid(m_nxtFrameID);

	for (auto iter : m_curFrameInfo)
	{
		uFrame = sFrame.add_users();
		uFrame->set_uuid(iter.first);

		for (auto v : iter.second)
			uFrame->add_keyinfo(v);
	}

	m_oldFrameInfo[m_curFrameID] = m_curFrameInfo;
	m_curFrameInfo.clear();

	int size = sFrame.ByteSize();

	knet::MessageBody s2cMsg;
	s2cMsg.size = size + knet::MessageBody::HEADER_TYPE_BYTES;
	s2cMsg.type = EMessageType::ESCFrame;

	sFrame.SerializeToArray(&s2cMsg.data, size);

	// broadcast
	Broadcast(&s2cMsg);
}