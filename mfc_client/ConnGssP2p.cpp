#include "stdafx.h"
#include "ConnGssP2p.h"
#include "Log.h"

void ConnGssP2p::on_create_complete(p2p_transport *transport, int status, void *user_data)
{
	LOGI_print("transport:%p status:%d", transport, status);
	ConnGssP2p* gssP2p = (ConnGssP2p*)user_data;
	if (status == 0)
	{
		gssP2p->IsConnected() = true;
		gssP2p->m_pTransport = transport;
	}
	else
	{
		gssP2p->IsConnected() = false;
		p2p_transport_destroy(gssP2p->m_pTransport);
		gssP2p->m_pTransport = NULL;
		LOGW_print("on_create_complete status:%d", status);
	}
}
void ConnGssP2p::on_disconnect_server(p2p_transport *transport, int status, void *user_data)
{
	LOGI_print("transport:%p status:%d", transport, status);
	ConnGssP2p* gssP2p = (ConnGssP2p*)user_data;
	gssP2p->IsConnected() = false;
	gss_client_pull_destroy(gssP2p->m_pTransport);
	gssP2p->m_pTransport = NULL;

	//清除音视频连接
	gssP2p->ClearAvClient();
}
void ConnGssP2p::on_connect_complete(p2p_transport *transport, int connection_id, int status, void *transport_user_data, void *connect_user_data)
{
	LOGI_print("transport:%p connection_id:%d status:%d", transport, connection_id, status);
	//加入音视频连接Map
	ConnGssP2p* gssP2p = (ConnGssP2p*)transport_user_data;
}
void ConnGssP2p::on_connection_disconnect(p2p_transport *transport, int connection_id, void *transport_user_data, void *connect_user_data)
{
	LOGI_print("transport:%p connection_id:%d", transport, connection_id);
	//找到影视片连接Map中的记录值，删除
	ConnGssP2p* gssP2p = (ConnGssP2p*)transport_user_data;
	gssP2p->KickAvClient(connection_id);
}
void ConnGssP2p::on_accept_remote_connection(p2p_transport *transport, int connection_id, int conn_flag, void *transport_user_data)
{
	LOGI_print("transport:%p connection_id:%d conn_flag:%d", transport, connection_id, conn_flag);
}
void ConnGssP2p::on_connection_recv(p2p_transport *transport, int connection_id, void *transport_user_data, void *connect_user_data, char* data, int len)
{
	//LOGI_print("transport:%p connection_id:%d", transport, connection_id);
	if (data == NULL || len == 0) return;
	GosFrameHead head;
	memcpy(&head, data + sizeof(P2pHead), sizeof(GosFrameHead));
	int type;
	if (head.nCodeType >= gos_audio_AAC)
	{
		type = 1;
	}
	else
	{
		type = 0;
	}

	ConnGssP2p* gssPull = (ConnGssP2p*)transport_user_data;

	char* pureData = data + sizeof(P2pHead)+sizeof(GosFrameHead);
	int pureDataLen = len - sizeof(P2pHead)-sizeof(GosFrameHead);

	if (type == 1){
		//static FILE* pAudio = NULL;
		//if (pAudio == NULL) pAudio = fopen("test.audio", "wb");
		//if (pAudio != NULL) fwrite(pureData, 1, pureDataLen, pAudio);
		return;
	}


	P2pDataFrame frame;
	//memcpy(frame.data, data, len);
	//frame.len = len;
	memcpy(frame.data, pureData, pureDataLen);
	frame.len = pureDataLen;
	frame.time_stamp = head.nTimestamp;
	frame.type = head.nCodeType;
	frame.user_data = connect_user_data;
	frame.transport = transport;

	if (gssPull->m_FrameBuffer.size() < 100)
	{
		WaitForSingleObject(gssPull->m_Mutex, INFINITE);
		gssPull->m_FrameBuffer.push(frame);
		ReleaseMutex(gssPull->m_Mutex);
	}
	else
	{
		LOGT_print("m_FrameBuffer.push error size > 100\n");
	}
}
void ConnGssP2p::on_tcp_proxy_connected(p2p_transport *transport, void *transport_user_data, void *connect_user_data, unsigned short port, char* addr)
{
	LOGI_print("transport:%p port:%d addr:%s", transport, port, addr);
}

void ConnGssP2p::on_dispatch_callback(void* dispatcher, int status, void* user_data, char* server, unsigned short port, unsigned int server_id)
{
	LOGI_print("dispatcher:%p status:%d server:%s port:%d", dispatcher, status, server, port);

	ConnGssP2p* gssP2p = (ConnGssP2p*)user_data;

	strcpy(gssP2p->m_CfgServer, server);
	gssP2p->m_CfgServerPort = port;
	gssP2p->m_Cfg.server = gssP2p->m_CfgServer;
	gssP2p->m_Cfg.port = gssP2p->m_CfgServerPort;

	int ret = p2p_transport_create(&gssP2p->m_Cfg, &gssP2p->m_pTransport);
	if (ret != P2P_SUCCESS)
	{
		LOGE_print("create p2p transport failed");
	}
	else
	{
		LOGI_print("m_pTransport:%p", gssP2p->m_pTransport);
	}
	destroy_p2p_dispatch_requester(dispatcher);
	gssP2p->m_pDispatcher = NULL;
}

ConnGssP2p::ConnGssP2p()
{
	m_pTransport = NULL;
	m_pDispatcher = NULL;
	memset(&m_Cfg, 0, sizeof(m_Cfg));
}


ConnGssP2p::~ConnGssP2p()
{
}

bool ConnGssP2p::Init(char* dispathce_server, char* server, unsigned short port, char* uid, char* pwd, int tcp)
{
	if (dispathce_server == NULL || server == NULL || uid == NULL)
	{
		LOGE_print("errer params");
		return false;
	}

	LOGI_print("dispatch:%s server:%s port:%d uid:%s", dispathce_server, server, port, uid);
	strcpy(m_DispatchServer, dispathce_server);
	strcpy(m_CfgServer, server);
	strcpy(m_Uid, uid);
	m_CfgServerPort = port;

	m_Cfg.server = m_CfgServer;
	m_Cfg.port = m_CfgServerPort;
	m_Cfg.user = m_Uid;//TODO 读取配置 获取UID
	m_Cfg.password = "";
	m_Cfg.use_tcp_connect_srv = tcp;
	m_Cfg.proxy_addr = "";
	m_Cfg.user_data = this;
	if (m_Cfg.user == NULL)//strlen(m_Cfg.user)==0
		m_Cfg.terminal_type = P2P_CLIENT_TERMINAL;
	else
		m_Cfg.terminal_type = P2P_DEVICE_TERMINAL;
	

	m_Cb.on_create_complete = on_create_complete;
	m_Cb.on_disconnect_server = on_disconnect_server;
	m_Cb.on_connect_complete = on_connect_complete;
	m_Cb.on_connection_disconnect = on_connection_disconnect;
	m_Cb.on_accept_remote_connection = on_accept_remote_connection;
	m_Cb.on_connection_recv = on_connection_recv;
	m_Cfg.cb = &m_Cb;

	return true;
}
bool ConnGssP2p::UnInit()
{
	return Stop();
}
bool ConnGssP2p::RecvMediaFrame(P2pDataFrame& frame)
{
	if (m_FrameBuffer.size() <= 0)
	{
		return false;
	}
	else
	{
		WaitForSingleObject(m_Mutex, INFINITE);
		frame = m_FrameBuffer.front();
		m_FrameBuffer.pop();
		ReleaseMutex(m_Mutex);
		return true;
	}
}

typedef struct
{
	unsigned int  channel;	// Camera Index
	unsigned char quality;	//refer to ENUM_QUALITY_LEVEL
	unsigned char reserved[3];
} SMsgAVIoctrlSetStreamCtrlReq, SMsgAVIoctrlGetStreamCtrlResq;
static char s_quelity = 0;
bool ConnGssP2p::Send(int id, CMD_TYPE_E type, void* data, int len)
{
	P2pHead head = PrivHeadFormat(0, type, 0, 1);
	char cmd[1024] = {0};
	int  cmd_len = 0;
	switch (type)
	{
	case CMD_START_VIDEO_START:
		memcpy(cmd, &head, sizeof(P2pHead));
		cmd_len = sizeof(P2pHead);
		break;
	case CMD_START_VIDEO_STOP:
		memcpy(cmd, &head, sizeof(P2pHead));
		cmd_len = sizeof(P2pHead);
		break;
	case CMD_START_AUDIO_START:
		memcpy(cmd, &head, sizeof(P2pHead));
		cmd_len = sizeof(P2pHead);
		break;
	case CMD_START_AUDIO_STOP:
		memcpy(cmd, &head, sizeof(P2pHead));
		cmd_len = sizeof(P2pHead);
		break;
	case IOTYPE_USER_IPCAM_SETSTREAMCTRL_REQ:
		memcpy(cmd, &head, sizeof(P2pHead));
		cmd_len += sizeof(P2pHead);
		SMsgAVIoctrlSetStreamCtrlReq req;
		req.quality = s_quelity==0?1:0;
		memcpy(cmd + cmd_len, &req, sizeof(SMsgAVIoctrlSetStreamCtrlReq));
		cmd_len += sizeof(SMsgAVIoctrlSetStreamCtrlReq);
		break;
	default:
		break;
	}

	int error_code;
	int ret = p2p_transport_send(m_pTransport, id, cmd, cmd_len, P2P_SEND_NONBLOCK, &error_code);
	if (error_code == 0)
	{
		LOGI_print("p2p_transport_send type:%d OK", type);
		return true;
	} 
	else
	{
		LOGE_print("p2p_transport_send type:%d Error:%d", type, error_code);
		return false;
	}
	
}
bool ConnGssP2p::Start()
{
	return true;
}
bool ConnGssP2p::Stop()
{
	ClearAvClient();
	ServerDisconnect();
	while (m_FrameBuffer.size() > 0)
	{
		WaitForSingleObject(m_Mutex, INFINITE);
		m_FrameBuffer.pop();
		ReleaseMutex(m_Mutex);
	}
	return true;
}

bool ConnGssP2p::ServerConnect(bool isClient, bool useDispatch)
{
	if (m_pTransport != NULL)
	{
		LOGW_print("p2p transport already created, destroy it first");
		return 0;
	}
	else
	{
		if (isClient)
			m_Cfg.terminal_type = P2P_CLIENT_TERMINAL;
		else
			m_Cfg.terminal_type = P2P_DEVICE_TERMINAL;

		if (useDispatch)
		{
			int ret = p2p_query_dispatch_server(this->m_Uid, this->m_DispatchServer, this,
				ConnGssP2p::on_dispatch_callback, &this->m_pDispatcher);
			if (ret != 0)
			{
				LOGE_print("p2p_query_dispatch_server error %s", ret);
				return -1;
			}
		}
		else
		{
			LOGW_print("force not use dispatch");
			int ret = p2p_transport_create(&m_Cfg, &m_pTransport);
			if (ret != P2P_SUCCESS)
			{
				LOGE_print("create p2p transport failed");
			}
			else
			{
				LOGI_print("m_pTransport:%p", m_pTransport);
			}
		}

		return 0;
	}
}

bool ConnGssP2p::ServerDisconnect()
{
	LOGW_print("m_pTransport:%p", m_pTransport);
	if (m_pTransport)
	{
		p2p_transport_destroy(m_pTransport);
		m_pTransport = NULL;
	}

	return true;
}

int  ConnGssP2p::CreateNewConnAv(char* uid)
{
	int connection_id;
	int ret = p2p_transport_connect(m_pTransport, uid, this, 0, &connection_id);
	if (ret == 0)
	{
		LOGI_print("p2p_transport_connect done uid:%s connection_id:%d", uid, connection_id);
		InsertAvClient(connection_id);
		return connection_id;
	} 
	else
	{
		LOGE_print("p2p_transport_connect error uid:%s connection_id:%d", uid, connection_id);
		return 0;
	}
}

bool ConnGssP2p::ClearAvClient()
{
	map<int, int>::iterator iter;

	for (iter = m_AvClient.begin(); iter != m_AvClient.end(); iter++)
	{
		int connId = iter->second;
		p2p_transport_disconnect(m_pTransport, connId);
	}
	m_AvClient.clear();
	return true;
}

bool ConnGssP2p::KickAvClient(int id)
{
	map<int, int>::iterator iter;

	iter = m_AvClient.find(id);
	if (iter != m_AvClient.end())
	{
		int connId = iter->second;
		p2p_transport_disconnect(m_pTransport, connId);
		m_AvClient.erase(connId);
		LOGI_print("p2p_transport_disconnect avClient:%d OK", id);
		return true;
	}
	else
	{
		LOGI_print("p2p_transport_disconnect avClient:%d Error", id);
		return false;
	}
}

bool ConnGssP2p::InsertAvClient(int id)
{
	
	pair<map<int, int>::iterator, bool> pair1 = m_AvClient.insert(pair<int, int>(id, id));
	if (pair1.second == true)
	{
		LOGI_print("InsertAvClient avClient:%d OK", id);
		return true;
	}
	else
	{
		LOGE_print("InsertAvClient avClient:%d Error", id);
		return false;
	}
}