#pragma once
#include "gss_transport.h"
#include "p2p_transport.h"
#include "p2p_dispatch.h"
#include <queue>
using namespace std;

typedef enum _gos_frame_type
{
	gos_unknown_frame = 0,				// δ֪֡
	gos_video_i_frame = 1,				// I ֡
	gos_video_p_frame = 2,				// P ֡
	gos_video_b_frame = 3,				// B ֡
	gos_video_cut_i_frame = 4,			//����¼��I֡
	gos_video_cut_p_frame = 5,			//����¼��P֡
	gos_video_cut_b_frame = 6,			//����¼��B֡
	gos_video_cut_end_frame = 7,		//����¼��B֡
	gos_video_preview_i_frame = 10,		//Ԥ��ͼ
	gos_video_end_frame,
	gos_audio_frame = 50,			// ��Ƶ֡
	gos_cut_audio_frame = 51, 		// ��Ƶ֡
	gos_special_frame = 100,		// ����֡	 gos_special_data �����ŵƵ�״̬�����ϴ�app
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
	int  			flag;		//��Ϣ��ʼ��ʶ
	unsigned int 	size;		//���շ�����Ϣ��С(��������Ϣͷ)
	char 			type;		//Э������1 json 2 json ����
	char			protoType;	//��Ϣ����1 ����2Ӧ��3֪ͨ
	int 			msgType;	//IOTYPE��Ϣ����
	char 			reserve[6];	//����
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
	unsigned int	nFrameNo;			// ֡��
	unsigned int	nFrameType;			// ֡����	gos_frame_type_t
	unsigned int	nCodeType;			// �������� gos_codec_type_t
	unsigned int	nFrameRate;			// ��Ƶ֡�ʣ���Ƶ������
	unsigned int	nTimestamp;			// ʱ���
	unsigned short	sWidth;				// ��Ƶ��
	unsigned short	sHeight;			// ��Ƶ��
	unsigned int	reserved;			// Ԥ��
	unsigned int	nDataSize;			// data���ݳ���
}GosFrameHead;

enum CMD_TYPE_E
{
	CMD_START_VIDEO_START					= 0x01FF,
	CMD_START_VIDEO_STOP					= 0x02FF,
	CMD_START_AUDIO_START					= 0x0300,
	CMD_START_AUDIO_STOP					= 0x0301,
	IOTYPE_USER_IPCAM_SETSTREAMCTRL_REQ		= 0x0320,	//�����л�
	IOTYPE_USER_IPCAM_SETSTREAMCTRL_RESP	= 0x0321,	//�����л�Ӧ��
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
			// 1. ��һ��WM_QUIT����Ϣ�ᡡUI���߳�  
			m_tdDispatch->PostThreadMessage(WM_QUIT, NULL, NULL);
			// 2. �ȴ���UI���߳������˳�  
			if (WAIT_OBJECT_0 == WaitForSingleObject(m_tdDispatch->m_hThread, INFINITE)){
				// 3. ɾ�� UI �̶߳���ֻ�е���������m_bAutoDelete = FALSE;��ʱ�ŵ���  
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

