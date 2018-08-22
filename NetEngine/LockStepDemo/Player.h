#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "client_socket.h"

#define DEF_FUNC_PARAM(func, type, var, dft) \
    private:\
        type var = dft;\
    public:\
        void Set##func(type tmp) {\
            var = tmp;\
        }\
        type Get##func() {\
            return var;\
        }

class Player
{
public:
	Player(const knet::ClientSocketPtr& client);
	virtual ~Player();

	void SendImm(knet::MessageBody* msg);

private:
	knet::ClientSocketPtr m_client;

	DEF_FUNC_PARAM(UUID, uint32_t, m_UUID, 0);
	DEF_FUNC_PARAM(PosX, double, m_posX, 0);
	DEF_FUNC_PARAM(PosY, double, m_posY, 0);
	DEF_FUNC_PARAM(PosZ, double, m_posZ, 0);
};

typedef std::shared_ptr<Player> PlayerPtr;

#endif // __PLAYER_H__