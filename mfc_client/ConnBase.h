#pragma once
#include "gss_transport.h"
#include "p2p_transport.h"
#include "p2p_dispatch.h"
#include <queue>
using namespace std;

typedef enum _gos_frame_type
{
	gos_unknown_frame = 0,				// 未知帧
	gos_video_i_frame = 1,				// I 帧
	gos_video_p_frame = 2,				// P 帧
	gos_video_b_frame = 3,				// B 帧
	gos_video_cut_i_frame = 4,			//剪接录像I帧
	gos_video_cut_p_frame = 5,			//剪接录像P帧
	gos_video_cut_b_frame = 6,			//剪接录像B帧
	gos_video_cut_end_frame = 7,		//剪接录像B帧
	gos_video_preview_i_frame = 10,		//预览图
	gos_video_end_frame,
	gos_audio_frame = 50,			// 音频帧
	gos_cut_audio_frame = 51, 		// 音频帧
	gos_special_frame = 100,		// 特殊帧	 gos_special_data 比如门灯灯状态主动上传app
} gos_frame_type_t;
typedef enum _gos_codec_type
{
	gos_codec_unknown = 0,
	gos_video_codec_start = 10,
	gos_video_H264_AAC,
	gos_video_H264_G711,
	gos_video_H265,
	gos_video_MPEG4,
	gos_video_MJPEG,
	gos_video_codec_end,
	gos_audio_codec_start = 50,
	gos_audio_AAC,
	gos_audio_G711A,
	gos_audio_G711U,
	gos_audio_codec_end,
} gos_codec_type_t;

#pragma   pack(1)
typedef struct GP2pHead_t
{
	int  			flag;		//消息开始标识
	unsigned int 	size;		//接收发送消息大小(不包括消息头)
	char 			type;		//协议类型1 json 2 json 加密
	char			protoType;	//消息类型1 请求2应答3通知
	int 			msgType;	//IOTYPE消息类型
	char 			reserve[6];	//保留
}P2pHead;
#pragma   pack()

typedef struct _p2p_data_frame
{
	void *transport;
	void *user_data;
	char data[256*1024];
	int len;
	char type;
	unsigned int time_stamp;
}P2pDataFrame;

typedef struct _ggos_frame_head
{
	unsigned int	nFrameNo;			// 帧号
	unsigned int	nFrameType;			// 帧类型	gos_frame_type_t
	unsigned int	nCodeType;			// 编码类型 gos_codec_type_t
	unsigned int	nFrameRate;			// 视频帧率，音频采样率
	unsigned int	nTimestamp;			// 时间戳
	unsigned short	sWidth;				// 视频宽
	unsigned short	sHeight;			// 视频高
	unsigned int	reserved;			// 预留
	unsigned int	nDataSize;			// data数据长度
}GosFrameHead;

enum CMD_TYPE_E
{
	CMD_START_VIDEO_START			= 0x01FF,
	CMD_START_VIDEO_STOP			= 0x02FF,
	CMD_START_AUDIO_START			= 0x0300,
	CMD_START_AUDIO_STOP			= 0x0301,
};

class ConnBase
{
public:
	ConnBase(){
		m_tdDispatchStop = false;
		m_tdDispatch = NULL;
	}
	virtual ~ConnBase(){
		if (m_tdDispatch != NULL){
			m_tdDispatchStop = true;
			// 1. 发一个WM_QUIT　消息结　UI　线程  
			m_tdDispatch->PostThreadMessage(WM_QUIT, NULL, NULL);
			// 2. 等待　UI　线程正常退出  
			if (WAIT_OBJECT_0 == WaitForSingleObject(m_tdDispatch->m_hThread, INFINITE)){
				// 3. 删除 UI 线程对象，只有当你设置了m_bAutoDelete = FALSE;　时才调用  
				delete m_tdDispatch;
				m_tdDispatch = NULL;
			}
		}
	}

	virtual bool Init(char* dispathce_server, char* server, unsigned short port, char* uid, char* pwd, int tcp) = 0;
	virtual bool UnInit() = 0;
	virtual bool RecvMediaFrame(P2pDataFrame& frame) = 0;
	virtual bool Send(int id, CMD_TYPE_E type, void* data, int len) = 0;
	virtual bool Start() = 0;
	virtual bool Stop() = 0;
	virtual bool& IsConnected(){ return m_isConnected; }

	P2pHead PrivHeadFormat(int size, int msgType, int type, int protoType)
	{
		P2pHead head;
		memset(&head, 0, sizeof(P2pHead));

		head.flag = 0x67736d80;
		head.size = size;
		head.type = type;
		head.protoType = protoType;
		head.msgType = msgType;

		return head;
	}
	//static UINT DispatchProcImpl(void* param);
	int					m_type;
	char				m_DispatchServer[64];
	char				m_CfgServer[64];
	char				m_CfgUser[64];
	char				m_CfgPwd[64];
	unsigned short		m_CfgServerPort;

protected:
	bool					m_isConnected;
	CWinThread*				m_tdDispatch;
	bool					m_tdDispatchStop;
	HANDLE					m_Mutex;
	queue<P2pDataFrame>		m_FrameBuffer;
};

