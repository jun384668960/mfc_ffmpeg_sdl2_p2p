#include "stdafx.h"
#include "ConnGssTcp.h"

void ConnGssTcp::on_connect_result(void *transport, void* user_data, int status)
{
}
void ConnGssTcp::on_disconnect(void *transport, void* user_data, int status)
{

}
void ConnGssTcp::on_recv(void *transport, void *user_data, char* data, int len)
{

}
void ConnGssTcp::on_device_disconnect(void *transport, void *user_data)
{

}

ConnGssTcp::ConnGssTcp()
:ConnBase()
{
}


ConnGssTcp::~ConnGssTcp()
{
}

bool ConnGssTcp::Init(char* dispathce_server, char* server, unsigned short port, char* uid, char* pwd, int tcp)
{
	return true;
}

bool ConnGssTcp::UnInit()
{
	return true;
}
bool ConnGssTcp::RecvMediaFrame(P2pDataFrame& frame)
{
	return true;
}
bool ConnGssTcp::Send(int id, CMD_TYPE_E type, void* data, int len)
{
	return true;
}
