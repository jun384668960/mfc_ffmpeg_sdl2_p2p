#include "stdafx.h"
#include "GenericDecode.h"
#include "Log.h"

void GenericDecode::GlobleInit()
{
	av_register_all();
	avcodec_register_all();
}

GenericDecode::GenericDecode()
{
	m_Mutex = CreateMutex(NULL, FALSE, "GenericDecode");
	m_Callback = NULL;
	m_VCodecId = AV_CODEC_ID_NONE;
	m_ACodecId = AV_CODEC_ID_NONE;
}


GenericDecode::~GenericDecode()
{
}

bool GenericDecode::GenericInitDecode(AVCodecID codecId)
{
	WaitForSingleObject(m_Mutex, INFINITE);
	m_Packet = av_packet_alloc();

	m_pVCodec = avcodec_find_decoder(codecId);
	if (!m_pVCodec){
		LOGE_print("Codec not found\n");
		ReleaseMutex(m_Mutex);
		return false;
	}

	m_pVCodecParserCtx = av_parser_init(m_pVCodec->id);
	if (!m_pVCodecParserCtx){
		LOGE_print("Could not allocate video parser context");
		ReleaseMutex(m_Mutex);
		return false;
	}

	m_pVCodecCtx = avcodec_alloc_context3(m_pVCodec);
	if (!m_pVCodecCtx){
		LOGE_print("Could not allocate video codec context\n");
		ReleaseMutex(m_Mutex);
		return false;
	}

	//if (m_pCodec->capabilities&AV_CODEC_CAP_TRUNCATED)
	//	m_pCodecCtx->flags |= AV_CODEC_FLAG_TRUNCATED;

	if (avcodec_open2(m_pVCodecCtx, m_pVCodec, NULL) < 0) {
		LOGE_print("Could not open codec\n");
		ReleaseMutex(m_Mutex);
		return false;
	}

	m_pFrame = av_frame_alloc();
	ReleaseMutex(m_Mutex);
	m_VCodecId = codecId;

	return true;
}

bool GenericDecode::GenericUninitDecode()
{
	WaitForSingleObject(m_Mutex, INFINITE);
	avcodec_close(m_pVCodecCtx);
	if (m_pVCodecCtx){
		av_free(m_pVCodecCtx);
		m_pVCodecCtx = NULL;
	}
	av_free_packet(m_Packet);

	if (m_pFrame){
		av_frame_free(&m_pFrame);
	}
	
	ReleaseMutex(m_Mutex);
	return true;
}

bool GenericDecode::DelayInitAudioDecode(AVCodecID codecId)
{
	if (m_ACodecId == codecId)
		return true;

	WaitForSingleObject(m_Mutex, INFINITE);

	m_pACodec = avcodec_find_decoder(codecId);
	if (!m_pACodec){
		LOGE_print("Codec not found codecId:%d", codecId);
		ReleaseMutex(m_Mutex);
		return false;
	}

	m_pACodecParserCtx = av_parser_init(m_pACodec->id);
	if (!m_pACodecParserCtx){
		LOGE_print("Could not allocate audio parser context");
		ReleaseMutex(m_Mutex);
		return false;
	}

	m_pACodecCtx = avcodec_alloc_context3(m_pACodec);
	if (!m_pACodecCtx){
		LOGE_print("Could not allocate audio codec context\n");
		ReleaseMutex(m_Mutex);
		return false;
	}

	if (avcodec_open2(m_pACodecCtx, m_pACodec, NULL) < 0) {
		LOGE_print("Could not open codec\n");
		ReleaseMutex(m_Mutex);
		return false;
	}
	ReleaseMutex(m_Mutex);
	m_ACodecId = codecId;

	return true;
}

AVFrame* GenericDecode::GenericMediaDecode(bool isVideo, const char* buf, const int buf_len)
{
	int ret;
	

	if (isVideo)
	{//สำฦต
		int ret = av_parser_parse2(m_pVCodecParserCtx, m_pVCodecCtx, &m_Packet->data, &m_Packet->size, (uint8_t *)buf, buf_len,
			AV_NOPTS_VALUE, AV_NOPTS_VALUE, 0);
		if (ret < 0)
		{
			LOGE_print("av_parser_parse2 error");
			ReleaseMutex(m_Mutex);
			return NULL;
		}
		m_Packet->data = (uint8_t *)buf;
		m_Packet->size = buf_len;

		WaitForSingleObject(m_Mutex, INFINITE);
		ret = avcodec_send_packet(m_pVCodecCtx, m_Packet);
		if (ret < 0) {
			//LOGE_print("avcodec_send_packet error");
			ReleaseMutex(m_Mutex);
			return NULL;
		}

		ret = avcodec_receive_frame(m_pVCodecCtx, m_pFrame);
		if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
		{
			//LOGE_print("avcodec_receive_frame error");
			ReleaseMutex(m_Mutex);
			return NULL;
		}
		else if (ret < 0) {
			ReleaseMutex(m_Mutex);
			return NULL;
		}
		if (m_Callback != NULL)
		{
			m_Callback(m_pFrame, isVideo);
		}
		ReleaseMutex(m_Mutex);
	}
	else
	{
		if (m_pACodecCtx == NULL || m_pACodecParserCtx == NULL)
		{
			return false;
		}
		else
		{
			//int ret = av_parser_parse2(m_pACodecParserCtx, m_pACodecCtx, &m_Packet->data, &m_Packet->size, (uint8_t *)buf, buf_len,
			//	AV_NOPTS_VALUE, AV_NOPTS_VALUE, 0);
			//if (ret < 0)
			//{
			//	LOGE_print("av_parser_parse2 error");
			//	ReleaseMutex(m_Mutex);
			//	return NULL;
			//}
			m_Packet->data = (uint8_t *)buf;
			m_Packet->size = buf_len;

			WaitForSingleObject(m_Mutex, INFINITE);
			ret = avcodec_send_packet(m_pACodecCtx, m_Packet);
			if (ret < 0) {
				//LOGE_print("avcodec_send_packet error");
				ReleaseMutex(m_Mutex);
				return NULL;
			}

			ret = avcodec_receive_frame(m_pACodecCtx, m_pFrame);
			if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
			{
				//LOGE_print("avcodec_receive_frame error");
				ReleaseMutex(m_Mutex);
				return NULL;
			}
			else if (ret < 0) {
				ReleaseMutex(m_Mutex);
				return NULL;
			}
			if (m_Callback != NULL)
			{
				m_Callback(m_pFrame, isVideo);
			}
			ReleaseMutex(m_Mutex);
		}
	}
	return m_pFrame;
}

bool GenericDecode::GenericSetDisplayCallback(DecodeCallback callback)
{
	m_Callback = callback;
	return true;
}