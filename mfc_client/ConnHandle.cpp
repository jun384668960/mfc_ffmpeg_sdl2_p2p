#include "stdafx.h"
#include "ConnHandle.h"
#include "p2p_transport.h"
#include "gss_transport.h"
#include "p2p_dispatch.h"

ConnHandle::ConnHandle()
{
}


ConnHandle::~ConnHandle()
{

}

bool ConnHandle::GlobleInit()
{
	int ret = p2p_init(NULL);
	if (ret != 0)
	{
		printf("p2p_init error");
		return false;
	}

	return true;
}
bool ConnHandle::GlobleExit()
{
	p2p_uninit();
	return true;
}

ConnBase* ConnHandle::CreateNewConn(int type)
{
	if (type == CONN_TYPE_TCP_1VN_PULL)
	{
		return new ConnGssPull();
	}
	else if (type == CONN_TYPE_P2P_1V1)
	{
		return new ConnGssP2p();
	}
}
