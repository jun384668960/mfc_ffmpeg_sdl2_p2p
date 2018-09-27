#pragma once
#include "ConnBase.h"
#include <map>
using namespace std;

class ConnGssP2p :
	public ConnBase
{
public:
	ConnGssP2p();
	virtual ~ConnGssP2p();

	virtual bool Init(char* dispathce_server, char* server, unsigned short port, char* uid, char* pwd, int tcp);
	virtual bool UnInit();
	virtual bool RecvMediaFrame(P2pDataFrame& frame);
	virtual bool Send(int id, CMD_TYPE_E type, void* data, int len);
	virtual bool Start();
	virtual bool Stop();

public:
	bool ServerConnect(bool isClient, bool useDispatch);
	bool ServerDisconnect();
	bool ClearAvClient();
	int  CreateNewConnAv(char* uid);

	static void on_create_complete(p2p_transport *transport, int status, void *user_data);
	static void on_disconnect_server(p2p_transport *transport, int status, void *user_data);
	static void on_connect_complete(p2p_transport *transport, int connection_id, int status, void *transport_user_data, void *connect_user_data);
	static void on_connection_disconnect(p2p_transport *transport, int connection_id, void *transport_user_data, void *connect_user_data);
	static void on_accept_remote_connection(p2p_transport *transport, int connection_id, int conn_flag, void *transport_user_data);
	static void on_connection_recv(p2p_transport *transport, int connection_id, void *transport_user_data, void *connect_user_data, char* data, int len);
	static void on_tcp_proxy_connected(p2p_transport *transport, void *transport_user_data, void *connect_user_data, unsigned short port, char* addr);
	static void on_dispatch_callback(void* dispatcher, int status, void* user_data, char* server, unsigned short port, unsigned int server_id);
protected:
	bool KickAvClient(int id);
	bool InsertAvClient(int id);

protected:
	p2p_transport_cb			m_Cb;
	p2p_transport_cfg			m_Cfg;
	p2p_transport*				m_pTransport;
	void*						m_pDispatcher;
	char						m_Uid[64];
	map<int, int>				m_AvClient;
};


