#pragma once
#include "ConnBase.h"

class ConnGssTcp : public ConnBase
{
public:
	ConnGssTcp();
	virtual ~ConnGssTcp();

	virtual bool Init(char* dispathce_server, char* server, unsigned short port, char* uid, char* pwd, int tcp);
	virtual bool UnInit();
	virtual bool RecvMediaFrame(P2pDataFrame& frame);
	virtual bool Send(int id, CMD_TYPE_E type, void* data, int len);

	static void on_connect_result(void *transport, void* user_data, int status);
	//disconnect from server, status is error code
	static void on_disconnect(void *transport, void* user_data, int status);
	//receive device data
	static void on_recv(void *transport, void *user_data, char* data, int len);
	//device disconnect from server
	static void on_device_disconnect(void *transport, void *user_data);
protected:
	gss_client_conn_cfg		m_Cfg;
	gss_client_conn_cb		m_Cb;
	bool					m_bSigConnected;
};

