#pragma once
#include "opencv.hpp"
extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavutil/avutil.h"
#include "libavutil/opt.h"
#include "libavutil/pixdesc.h"
#include "libswscale/swscale.h"
#include "libavfilter/avfiltergraph.h"
#include "libavcodec/avcodec.h"
#include "libavfilter/buffersink.h"
#include "libavfilter/buffersrc.h"
#include "libavutil/audio_fifo.h"
#include "libswresample/swresample.h"
};


using namespace cv;


class Decoder
{
public:
	Decoder();
	virtual ~Decoder();

	int initDecoder(string url);
	int decode(AVPacket *pkt, Mat &img);
	int getImg(Mat &img);
private:
	AVFormatContext *m_pifmt_ctx;
	int m_videoid;
	AVCodecContext *m_pvideodec;
	uint8_t *m_pout_bufferRGB;
	SwsContext *m_img_yuv2rgb_ctx;
	int m_RGBsize;
};

//class Encoder
//{
//public:
//	Encoder();
//	int initEncoder(string url, Size videosize, int fps);
//	void fill_yuv_image(AVFrame *pFrameYUV, Mat &img);
//	int encode();
//private:
//	AVFrame *pFrameRGB = NULL;
//	SwsContext *m_img_rgb2yuv_ctx = NULL;
//	AVCodecContext *m_videoenc;
//	AVCodecContext *m_audioenc;
//	uint8_t  *out_bufferYUV;
//	SwsContext *m_img_rgb2yuv_ctx = NULL;
//
//	AVFormatContext *m_ofmt_ctx = NULL;
//	int m_videoid;
//};