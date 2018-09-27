#include "stdafx.h"
#include "ConnGssPull.h"
#include "Log.h"
#include "GenericDecode.h"

static GenericDecode* gGenericDecode = NULL;

void ConnGssPull::on_connect_result(void *transport, void* user_data, int status)
{
	LOGI_print("on_connect_result status:%d", status);
	ConnGssPull* gssPull = (ConnGssPull*)user_data;
	if (status == 0)
	{
		gssPull->IsConnected() = true;
		gssPull->m_Transport = transport;
	}
	else
	{
		gssPull->IsConnected() = false;
		gss_client_pull_destroy(gssPull->m_Transport);
		gssPull->m_Transport = NULL;
		LOGW_print("on_connect_result status:%d", status);
	}
}
void ConnGssPull::on_disconnect(void *transport, void* user_data, int status)
{
	LOGI_print("on_disconnect status:%d", status);
	ConnGssPull* gssPull = (ConnGssPull*)user_data;
	gssPull->IsConnected() = false;
	gss_client_pull_destroy(gssPull->m_Transport);
	gssPull->m_Transport = NULL;
}
void ConnGssPull::on_recv(void *transport, void *user_data, char* data, int len, char type, unsigned int time_stamp)
{
	//�����ݷ��뻺����
	//LOGT_print("type:%d time_stamp:%u len:%d\n", type, time_stamp, len);
	if (type == 1 || data == NULL || len == 0) return;

	GosFrameHead head;
	memcpy(&head, data + sizeof(P2pHead), sizeof(GosFrameHead));
	//P2pHead hd;
	//memcpy(&hd, data, sizeof(P2pHead));
	//LOGI_print("type:%d msgType:0x%02x  time_stamp:%u size:%d", type, hd.msgType, time_stamp, hd.size);
	//if (hd.msgType == 0xf6)
	//{
	//	return;
	//} 
	//else
	//{
	//	return;
	//}

	ConnGssPull* gssPull = (ConnGssPull*)user_data;

	char* pureData = data + sizeof(P2pHead)+sizeof(GosFrameHead);
	int pureDataLen = len - sizeof(P2pHead)-sizeof(GosFrameHead);
	P2pDataFrame frame;
	//memcpy(frame.data, data, len);
	//frame.len = len;
	memcpy(frame.data, pureData, pureDataLen);
	frame.len = pureDataLen;
	frame.time_stamp = time_stamp;
	frame.type = head.nCodeType;
	frame.user_data = user_data;
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

void ConnGssPull::on_device_disconnect(void *transport, void *user_data)
{
	LOGI_print("on_device_disconnect transport:%p", transport);
	ConnGssPull* gssPull = (ConnGssPull*)user_data;
	gssPull->IsConnected() = false;
	gss_client_pull_destroy(gssPull->m_Transport);
	gssPull->m_Transport = NULL;
}

void ConnGssPull::on_dispatch_callback(void* dispatcher, int status, void* user_data, char* server, unsigned short port, unsigned int server_id)
{
	LOGI_print("dispatcher:%p status:%d server:%s port:%d", dispatcher, status, server, port);

	ConnGssPull* gssPull = (ConnGssPull*)user_data;

	strcpy(gssPull->m_CfgServer, server);
	gssPull->m_CfgServerPort = port;
	gssPull->m_Cfg.server = gssPull->m_CfgServer;
	gssPull->m_Cfg.port = gssPull->m_CfgServerPort;

	destroy_p2p_dispatch_requester(dispatcher);
	gssPull->m_pDispatcher = NULL;
}

bool ConnGssPull::RecvMediaFrame(P2pDataFrame& frame)
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

ConnGssPull::ConnGssPull()
{
	m_Transport = NULL;
	m_Mutex = CreateMutex(NULL, FALSE, "ConnGssPull");
}

ConnGssPull::~ConnGssPull()
{
	UnInit();
}

bool ConnGssPull::Init(char* dispathce_server, char* server, unsigned short port, char* uid, char* pwd, int tcp)
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
	m_Cfg.user_data = this;
	m_Cfg.uid = m_Uid;

	m_Cb.on_connect_result = on_connect_result;
	m_Cb.on_disconnect = on_disconnect;
	m_Cb.on_recv = on_recv;
	m_Cb.on_device_disconnect = on_device_disconnect;
	m_Cfg.cb = &m_Cb;

	return true;
}

bool ConnGssPull::ServerConnect(bool isClient, bool useDispatch)
{
	if (m_Transport != NULL)
	{
		LOGW_print("p2p transport already created, destroy it first");
		return 0;
	}
	else
	{
		if (useDispatch)
		{
			int ret = p2p_query_dispatch_server(this->m_Uid, this->m_DispatchServer, this,
				ConnGssPull::on_dispatch_callback, &this->m_pDispatcher);
			if (ret != 0)
			{
				LOGE_print("p2p_query_dispatch_server error %s", ret);
				return -1;
			}
		}
		else
		{
			LOGW_print("force not use dispatch");
		}

		return 0;
	}
}

bool ConnGssPull::Start()
{
	int ret = gss_client_pull_connect(&m_Cfg, &m_Transport);
	if (ret != 0)
	{
		LOGE_print("gss_client_pull_connect error");
		return false;
	}

	return true;
}

bool ConnGssPull::Stop()
{
	if (m_Transport != NULL)
	{
		gss_client_pull_destroy(m_Transport);
		m_Transport = NULL;
		IsConnected() = false;
	}

	while (m_FrameBuffer.size() > 0)
	{
		WaitForSingleObject(m_Mutex, INFINITE);
		m_FrameBuffer.pop();
		ReleaseMutex(m_Mutex);
	}

	return true;
}

bool ConnGssPull::UnInit()
{
	return Stop();
}

bool ConnGssPull::Send(int id, CMD_TYPE_E type, void* data, int len)
{
	return true;
}