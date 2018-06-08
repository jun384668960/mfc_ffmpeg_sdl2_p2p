#pragma once
extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavutil/avutil.h"
#include "libswscale/swscale.h"
#include "libswresample/swresample.h"
}

#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "swresample.lib")
#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "swscale.lib")
#pragma comment(lib, "avutil.lib")

typedef void(*DecodeCallback)(AVFrame* pFrame, bool isVideo);
class GenericDecode
{
public:
	GenericDecode();
	virtual ~GenericDecode();

	bool GenericInitDecode(AVCodecID id);
	bool GenericUninitDecode();
	bool DelayInitAudioDecode(AVCodecID codecId);
	AVFrame* GenericMediaDecode(bool isVideo, const char* buf, const int buf_len);
	bool GenericSetDisplayCallback(DecodeCallback callback);
	static void GlobleInit();

protected:
	AVCodec*				m_pVCodec;
	AVCodec*				m_pACodec;
	AVCodecContext*			m_pVCodecCtx;
	AVCodecContext*			m_pACodecCtx;
	AVCodecParserContext*	m_pVCodecParserCtx;
	AVCodecParserContext*	m_pACodecParserCtx;
	AVCodecID				m_VCodecId;
	AVCodecID				m_ACodecId;
	AVPacket*				m_Packet;
	
	HANDLE					m_Mutex;
	AVFrame *				m_pFrame;
	DecodeCallback			m_Callback;
};

