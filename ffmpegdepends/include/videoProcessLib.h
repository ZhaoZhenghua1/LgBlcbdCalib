#pragma once
#include <string>
#include "opencv2/opencv.hpp"
#include "opencv2/opencv.hpp"
#include <atlstr.h>
#include <io.h>
#include <fcntl.h>
#include<time.h>

extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavutil/avutil.h"
#include "libavutil/opt.h"
#include "libavutil/pixdesc.h"
#include "libswscale/swscale.h"
#include "libavfilter/avfiltergraph.h"
	//#include "libavfilter/avcodec.h"
#include "libavfilter/buffersink.h"
#include "libavfilter/buffersrc.h"
#include "libavutil/audio_fifo.h"
#include "libswresample/swresample.h"
};


using namespace std;
using namespace cv;


class CTimeout_handler
{
public:
	CTimeout_handler(unsigned int t)
	{
		freq = getTickFrequency();
		timeout_ms_ = (float)freq * 100;
		lastTime_ = getTickCount();
	}

	void reset(unsigned int TimeoutMs = 1000)
	{
		timeout_ms_ = (float)TimeoutMs / 1000 * freq;
		lastTime_ = getTickCount();
	}

	bool is_timeout()
	{
		const double actualDelay = getTickCount() - lastTime_;
		return actualDelay > timeout_ms_;
	}

	static int check_interrupt(void * t)
	{
		return t && static_cast<CTimeout_handler *>(t)->is_timeout();
	}

public:
	unsigned int timeout_ms_;
	double lastTime_;
	double freq;
};

class CVideoDecoder 
{
public:
	CVideoDecoder();
	~CVideoDecoder();
	int initDecoder(string url);
	void closeDecoder();
	int decode(AVPacket *pkt, AVFrame **frame);
	int getFrame2(AVFrame **frame);
	void Frame2ImgResize(Mat &img, AVFrame *frame);
	void setWriter(AVFormatContext *pifmt_ctx, AVFormatContext *pifmt_ctx_sub);
	void writeAudio(AVPacket *pkt, int stream_index);
	void cropframe(AVFrame *dstframe, AVFrame *srcframe, Point pos, AVPixelFormat format);
	void Img2Frame(Mat &img, AVFrame *frame);
	void Frame2Img(Mat &img, AVFrame *frame);
	void showframe(AVFrame *frame, string name);


	AVCodecContext *m_audiodeodec;
	bool m_hasaudio = false;
	bool m_hasvideo = false;
	AVFormatContext *m_pifmt_ctx;
	int m_videoid;

	int m_pixformat;


private:
	AVFormatContext *m_pofmt_ctx;
	AVFormatContext *m_pofmt_ctx_sub;

	AVCodecContext *m_pvideodec;

	CTimeout_handler *m_th;
	Size m_procsize;
	bool m_bShouldWrite = false;

	bool m_bFirstWriteLog = true;

	//AVBitStreamFilterContext* aacBitstreamFilterContext;
};


class CWriteFrame
{
public:
	CWriteFrame();
	~CWriteFrame();

	int initWriter(string videoname, bool hasaudio, CVideoDecoder *decoder, Size videosize, int fps);
	void closeWriter();

	int writeFrame(AVFrame *frame);
	AVFormatContext *m_ofmt_ctx;

private:
	string m_videoname;
	Size m_savesize;
	AVCodecContext *m_videoenc;
	AVStream *m_videost;

	AVRational inst_timebase;

	int frameid = 0;
};



int __stdcall WriteLogtxt(char *fmt, ...);
void charToTCHAR(const char* strIn, TCHAR* strOut, DWORD dType = CP_ACP);			//char转TCHAR
void TcharToChar(const TCHAR* tcStr, char* cStr, DWORD dType = CP_ACP);