#include "cell_network.h"

#include "net_defined.hpp"

namespace knet
{
	CellNetwork::CellNetwork()
	{
	}

	CellNetwork::~CellNetwork()
	{
	}

	void CellNetwork::Init()
	{
#ifdef _WIN32
		WORD ver = MAKEWORD(2, 2);
		WSADATA data;
		WSAStartup(ver, &data);
#else
		signal(SIGPIPE, SIG_IGN);
#endif
	}

	void CellNetwork::Fini()
	{
#ifdef _WIN32
		WSACleanup();
#endif
	}

}; // end of namespace knet