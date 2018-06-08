#pragma once
#include "ConnGssPull.h"
#include "ConnGssP2p.h"

#define CONN_TYPE_TCP_1VN_PULL	0
#define CONN_TYPE_TCP_1V1		1
#define CONN_TYPE_P2P_1V1		2
class ConnHandle
{
public:
	ConnHandle();
	virtual ~ConnHandle();
	
	static bool GlobleInit();
	static bool GlobleExit();

	static ConnBase* CreateNewConn(int type);
protected:

private:
};

