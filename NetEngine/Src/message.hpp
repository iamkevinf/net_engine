#ifndef __MESSAGE_HPP__
#define __MESSAGE_HPP__

namespace knet
{
	enum class MessageType
	{
		MT_C2S_LOGIN,
		MT_S2C_LOGIN,

		MT_C2S_LOGOUT,
		MT_S2C_LOGOUT,

		MT_S2C_JOIN,

		MT_ERROR
	};

	struct DataHeader
	{
		DataHeader()
		{
			dataLen = sizeof(DataHeader);
			cmd = MessageType::MT_ERROR;
		}

		short dataLen;
		MessageType cmd;
	};

	struct c2s_Login : public DataHeader
	{
		c2s_Login()
		{
			dataLen = sizeof(c2s_Login);
			cmd = MessageType::MT_C2S_LOGIN;
		}

		char userName[32];
		char passWord[32];
		char data[928];
	};

	struct s2c_Login : public DataHeader
	{
		s2c_Login()
		{
			dataLen = sizeof(s2c_Login);
			cmd = MessageType::MT_S2C_LOGIN;
			ret = 0;
		}

		char userName[32];
		int ret;
		char data[956];
	};


	struct c2s_Logout : public DataHeader
	{
		c2s_Logout()
		{
			dataLen = sizeof(c2s_Logout);
			cmd = MessageType::MT_C2S_LOGOUT;
		}

		char userName[32];
	};

	struct s2c_Logout : public DataHeader
	{
		s2c_Logout()
		{
			dataLen = sizeof(s2c_Logout);
			cmd = MessageType::MT_S2C_LOGOUT;
			ret = 0;
		}

		int ret;
		char userName[32];
	};

	struct s2c_Join : public DataHeader
	{
		s2c_Join()
		{
			dataLen = sizeof(s2c_Join);
			cmd = MessageType::MT_S2C_JOIN;
			sock = 0;
		}

		int sock;
	};

}; // end of namespace knet

#endif // __MESSAGE_HPP__