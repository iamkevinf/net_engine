#include "Player.h"

Player::Player(const knet::ClientSocketPtr& client):m_client(client)
{

}

Player::~Player()
{
}

void Player::SendImm(knet::MessageBody* msg)
{
	m_client->SendImm(msg);
}
