#pragma once
#include "ConnBase.h"

class ConnGssPull :
	public ConnBase
{
public:
	ConnGssPull();
	virtual ~ConnGssPull();

	virtual bool Init(char* dispathce_server, char* server, unsigned short port, char* uid, char* pwd, int tcp);
	virtual bool UnInit();
	virtual bool RecvMediaFrame(P2pDataFrame& frame);
	virtual bool Send(int id, CMD_TYPE_E type, void* data, int len);
	virtual bool Start();
	virtual bool Stop();
	virtual bool ServerConnect(bool isClient, bool useDispatch);

	//connect device result, status is 0 ok
	static void on_connect_result(void *transport, void* user_data, int status);
	//disconnect from server, status is error code
	static void on_disconnect(void *transport, void* user_data, int status);
	//receive device data
	//type 0 video, 1 audio
	static void on_recv(void *transport, void *user_data, char* data, int len, char type, unsigned int time_stamp);
	//device disconnect from server
	static void on_device_disconnect(void *transport, void *user_data);
	static void on_dispatch_callback(void* dispatcher, int status, void* user_data, char* server, unsigned short port, unsigned int server_id);
protected:
	gss_pull_conn_cfg		m_Cfg;
	gss_pull_conn_cb		m_Cb;
	void*					m_Transport;
	void*					m_pDispatcher;
	char					m_Uid[64];
};

