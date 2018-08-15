#include "World.h"

#include "Proto/message_type.pb.h"

World::World()
{
	REGISTER_CMD_CALLBACK(EMessageType::ECSLogin, HandleLogin);
}

World::~World()
{
}


void World::Start()
{

}

void World::Tick()
{
	MessageProc();
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

	std::cout << "HandleLogin cur uuid: " << player->GetUUID()
		<< " username: " << proto.username()
		<< std::endl;

	SCLogin ret;
	ret.set_ret(true);
	size = ret.ByteSize();

	knet::MessageBody s2cMsg;
	s2cMsg.size = size + knet::MessageBody::HEADER_TYPE_BYTES;
	s2cMsg.type = EMessageType::ESCLogin;

	ret.SerializeToArray(&s2cMsg.data, size);

	player->SendImm(&s2cMsg);

	return true;
}