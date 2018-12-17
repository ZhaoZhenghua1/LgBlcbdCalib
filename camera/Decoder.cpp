#include "Decoder.h"

Decoder::Decoder()
{
	m_pout_bufferRGB = NULL;
	m_pifmt_ctx = NULL;
}

Decoder::~Decoder()
{
	if (m_pout_bufferRGB != NULL)
		delete[] m_pout_bufferRGB;

	if (m_pifmt_ctx != NULL)
	{
		for (int i = 0; i < m_pifmt_ctx->nb_streams; i++)
		{
			avcodec_close(m_pifmt_ctx->streams[i]->codec);
		}
		avformat_close_input(&m_pifmt_ctx);
	}
}

int Decoder::initDecoder(string url)
{
	const char* p = url.c_str();
	av_register_all();
	avformat_network_init();

	int ret = 0;
	m_pifmt_ctx = avformat_alloc_context();

	AVDictionary* options = NULL;
	av_dict_set(&options, "rtsp_transport", "tcp", 0);
	//////// avformat_open_input默认是阻塞的，添加超时设置，避免一直等待 //////////////// 
	av_dict_set(&options, "stimeout", "5000000", 0);// 时间单位为微秒us

	//m_pifmt_ctx->probesize *= 10;
	if ((ret = avformat_open_input(&m_pifmt_ctx, url.c_str(), 0, &options)) < 0)
	{
		char buf[1024];
		av_strerror(ret, buf, 1024);
		throw std::string("Could not open input file.") + url + buf;
		return -1;
	}

	m_pifmt_ctx->probesize = 100 * 1024;
	m_pifmt_ctx->duration = 10 * AV_TIME_BASE;
	if ((ret = avformat_find_stream_info(m_pifmt_ctx, 0)) < 0) {
		throw std::string("Failed to retrieve input stream information");
		return -1;
	}

	for (int i = 0; i<m_pifmt_ctx->nb_streams; i++)
		if (m_pifmt_ctx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
			m_videoid = i;
			continue;
		}

	av_dump_format(m_pifmt_ctx, 0, url.c_str(), 0);

	m_pvideodec = m_pifmt_ctx->streams[m_videoid]->codec;

	if (avcodec_open2(m_pvideodec, avcodec_find_decoder(m_pvideodec->codec_id), NULL) < 0)
	{
		return  -1;
	}

	m_RGBsize = avpicture_get_size(AV_PIX_FMT_BGR24, m_pvideodec->width, m_pvideodec->height);
	if (m_RGBsize <= 0)
	{
		m_RGBsize = 1000;
	}
	m_pout_bufferRGB = new uint8_t[m_RGBsize];
	m_img_yuv2rgb_ctx = sws_getContext(m_pvideodec->width, m_pvideodec->height, AV_PIX_FMT_YUV420P, m_pvideodec->width, m_pvideodec->height, AV_PIX_FMT_BGR24, SWS_BICUBIC, NULL, NULL, NULL);

	return 0;
}

int Decoder::decode(AVPacket *pkt, Mat &img)
{
	int ret = 0;
	int stream_index = pkt->stream_index;

	if (stream_index == m_videoid)
	{
		AVFrame *frame = av_frame_alloc();
		int got_pic;

		ret = avcodec_decode_video2(m_pifmt_ctx->streams[stream_index]->codec, frame, &got_pic, pkt);

		if (ret < 0)
		{
			av_frame_free(&frame);
			return 0;
		}
		if (got_pic)
		{
			AVFrame *pFrameRGB = NULL;
			pFrameRGB = av_frame_alloc();

			if (img.empty())
				img.create(m_pvideodec->height, m_pvideodec->width, CV_8UC3);

			avpicture_fill((AVPicture*)pFrameRGB, m_pout_bufferRGB, AV_PIX_FMT_BGR24, m_pvideodec->width, m_pvideodec->height);
			sws_scale(m_img_yuv2rgb_ctx, frame->data, frame->linesize, 0, m_pvideodec->height, pFrameRGB->data, pFrameRGB->linesize);

			memcpy(img.data, m_pout_bufferRGB, m_RGBsize);

			av_frame_free(&pFrameRGB);
			ret = 1;
		}
		else
		{
			ret = 0;
		}

		av_frame_free(&frame);
	}
	return ret;
}

int Decoder::getImg(Mat &img)
{
	if (!m_pifmt_ctx)
	{
		return -1;
	}
	AVPacket pkt;
	int ret = av_read_frame(m_pifmt_ctx, &pkt);
	if (ret < 0)
		return ret;
	ret = decode(&pkt, img);
	if (ret <= 0)
	{
		av_free_packet(&pkt);
		return ret;
	}
	av_free_packet(&pkt);
	return 1;
}

//Encoder::Encoder()
//{
//
//}
//
//int Encoder::initEncoder(string url, Size videosize, int fps)
//{
//	pFrameRGB = avcodec_alloc_frame();
//	int YUVsize = avpicture_get_size(AV_PIX_FMT_YUV420P, videosize.width, videosize.height);
//	int RGBsize = avpicture_get_size(AV_PIX_FMT_BGR24, videosize.width, videosize.height);
//
//	out_bufferYUV = new uint8_t[YUVsize];
//
//	avformat_alloc_output_context2(&m_ofmt_ctx, NULL, NULL, url.c_str());
//	if (!m_ofmt_ctx)
//	{
//		printf("Could not create output context\n");
//		return false;
//	}
//
//	AVFormatContext *pinformatc = m_gifmt_ctx;
//	for (int i = 0; i < 2; i++)
//	{
//		in_stream = pinformatc->streams[i];
//
//		if (in_stream->codec->codec_type == AVMEDIA_TYPE_AUDIO)
//		{
//			m_audioid = i;
//			AVCodecContext *audiodec = in_stream->codec;
//
//			//需要解码后重新编码
//			if (m_pvideoIO->shouldrecode)
//			{
//				//Create output AVStream according to input AVStream
//				out_stream = avformat_new_stream(m_ofmt_ctx, NULL);
//				if (!out_stream)
//				{
//					printf("Failed allocating output stream\n");
//					ret = AVERROR_UNKNOWN;
//					return false;
//				}
//
//				m_audioenc = out_stream->codec;
//
//				encoder = avcodec_find_encoder(AV_CODEC_ID_AAC);
//				//m_audioenc->sample_rate = audiodec->sample_rate;
//				m_audioenc->sample_rate = audiodec->sample_rate;
//				//m_audioenc->sample_rate = 48000;
//				m_audioenc->channel_layout = audiodec->channel_layout;
//				//m_audioenc->channel_layout = 3;
//				m_audioenc->channels = av_get_channel_layout_nb_channels(m_audioenc->channel_layout);
//				/* take first format from list of supported formats */
//				m_audioenc->sample_fmt = encoder->sample_fmts[0];
//				AVRational time_base = { 1, m_audioenc->sample_rate };
//				m_audioenc->time_base = time_base;
//
//				m_audioenc->thread_count = 4;
//
//				ret = avcodec_open2(m_audioenc, encoder, NULL);
//				if (ret < 0)
//				{
//					av_log(NULL, AV_LOG_ERROR, "Cannot open video encoder for stream #%u\n", i);
//					return ret;
//				}
//			}
//			else if (m_pvideoIO->hasaudio) //不需要编解码
//			{
//				out_stream = avformat_new_stream(m_ofmt_ctx, in_stream->codec->codec);
//				if (!out_stream)
//				{
//					printf("Failed allocating output stream\n");
//					ret = AVERROR_UNKNOWN;
//					return false;
//				}
//				//Copy the settings of AVCodecContext
//				if (avcodec_copy_context(out_stream->codec, in_stream->codec) < 0)
//				{
//					printf("Failed to copy context from input to output stream codec context\n");
//					return false;
//				}
//				out_stream->codec->codec_tag = 0;
//			}
//		}
//		if (in_stream->codec->codec_type == AVMEDIA_TYPE_VIDEO)
//		{
//			m_videoid = i;
//
//			//Create output AVStream according to input AVStream
//			out_stream = avformat_new_stream(m_ofmt_ctx, NULL);
//			if (!out_stream)
//			{
//				printf("Failed allocating output stream\n");
//				ret = AVERROR_UNKNOWN;
//				return false;
//			}
//
//			AVCodecContext *videodec = in_stream->codec;
//			m_videoenc = out_stream->codec;
//
//			encoder = avcodec_find_encoder(AV_CODEC_ID_H264);
//			m_videoenc->height = videosize.height;
//			m_videoenc->width = videosize.width;
//			m_videoenc->codec_id = AV_CODEC_ID_H264;
//			m_videoenc->sample_aspect_ratio = videodec->sample_aspect_ratio;
//			m_videoenc->pix_fmt = encoder->pix_fmts[0];
//			m_videoenc->time_base.den = in_stream->r_frame_rate.num;
//			m_videoenc->time_base.num = in_stream->r_frame_rate.den;
//
//			//m_videoenc->time_base.den = 25;
//			//m_videoenc->time_base.num = 1;
//			//m_videoenc->time_base = videodec->time_base;
//			m_videoenc->codec_type = AVMEDIA_TYPE_VIDEO;
//
//			out_stream->r_frame_rate = in_stream->r_frame_rate;
//
//			m_videoenc->qmax = 25;
//			m_videoenc->qmin = 12;
//
//			//后加
//			m_videoenc->max_b_frames = 0;
//			//m_videoenc->flags = 0;
//			m_videoenc->me_method = 0;
//			m_videoenc->me_subpel_quality = 0;
//			m_videoenc->thread_count = 4;
//
//			//encoder = avcodec_find_encoder(videodec->codec_id);
//			//videoenc->height = videodec->height;
//			//videoenc->width = videodec->width;
//			//videoenc->sample_aspect_ratio = videodec->sample_aspect_ratio;
//			///* take first format from list of supported formats */
//			//videoenc->pix_fmt = encoder->pix_fmts[0];
//			///* video time_base can be set to whatever is handy and supported by encoder */
//			//videoenc->time_base = videodec->time_base;
//
//			////不懂
//			m_videoenc->me_range = 16;
//			m_videoenc->max_qdiff = 4;
//			m_videoenc->qcompress = 0.8;
//
//			ret = avcodec_open2(m_videoenc, encoder, NULL);
//			if (ret < 0)
//			{
//				return false;
//			}
//		}
//
//		if (m_ofmt_ctx->oformat->flags & AVFMT_GLOBALHEADER)
//			out_stream->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;
//	}
//
//	//Output information------------------
//	av_dump_format(m_ofmt_ctx, 0, Videoname.c_str(), 1);
//
//	//Open output file
//	AVOutputFormat *ofmt = m_ofmt_ctx->oformat;
//	if (!(ofmt->flags & AVFMT_NOFILE))
//	{
//		ret = avio_open(&m_ofmt_ctx->pb, Videoname.c_str(), AVIO_FLAG_WRITE);
//		if (ret < 0)
//		{
//			printf("Could not open output file '%s'\n", Videoname.c_str());
//			return false;
//		}
//	}
//
//
//	/*av_opt_set(m_ofmt_ctx->priv_data, "preset", "superfast", 0);
//	av_opt_set(m_ofmt_ctx->priv_data, "tune", "zerolatency", 0);*/
//	ret = avformat_write_header(m_ofmt_ctx, NULL);
//	if (ret < 0)
//	{
//		printf("Error occurred when opening output file\n");
//		return false;
//	}
//
//	DWORD dwThreadID;
//	m_writethread = CreateThread(NULL, 0, WriteThreadProc, (LPVOID)this, 0, &dwThreadID);
//	if (NULL == m_writethread)
//	{
//		printf("CreateThread failed (%d)\n", GetLastError());
//		return false;
//	}
//
//	if (m_videoid < 0)
//	{
//		printf("缺少视频信息\n");
//		return false;
//	}
//	if (m_audioid < 0)
//	{
//		printf("缺少音频信息\n");
//		m_pvideoIO->hasaudio = false;
//	}
//
//
//	m_pvideoIO->m_gofmt_ctx = m_ofmt_ctx;
//
//	SetEvent(m_pvideoIO->m_hStartSave);
//
//	return true;
//}
//
//void Encoder::fill_yuv_image(AVFrame *pFrameYUV, Mat &img)
//{
//	if (m_img_rgb2yuv_ctx == NULL)
//	{
//		m_img_rgb2yuv_ctx = sws_getContext(img.cols, img.rows,
//			AV_PIX_FMT_BGR24, m_videoenc->width, m_videoenc->height,
//			m_videoenc->pix_fmt, SWS_BICUBIC, NULL, NULL, NULL);
//	}
//
//	avpicture_fill((AVPicture *)pFrameYUV, out_bufferYUV, AV_PIX_FMT_YUV420P, m_videoenc->width, m_videoenc->height);
//	avpicture_fill((AVPicture *)pFrameRGB, img.data, AV_PIX_FMT_BGR24, m_videoenc->width, m_videoenc->height);
//
//	sws_scale(m_img_rgb2yuv_ctx, pFrameRGB->data, pFrameRGB->linesize, 0, m_videoenc->height, pFrameYUV->data, pFrameYUV->linesize);
//
//	pFrameYUV->width = img.cols;
//	pFrameYUV->height = img.rows;
//	pFrameYUV->format = AV_PIX_FMT_YUV420P;
//	//av_frame_free(&pFrameRGB);
//	//delete[] out_bufferYUV;
//}
//
//int Encoder::encode(Mat &img)
//{
//	AVCodecContext *videoenc = m_ofmt_ctx->streams[m_videoid]->codec;
//	AVPacket pkt;
//	int got_output;
//	av_init_packet(&pkt);
//	pkt.data = NULL;    // packet data will be allocated by the encoder
//	pkt.size = 0;
//	AVFrame *pFrameYUV = avcodec_alloc_frame();
//
//	static double freq = getTickFrequency();
//
//	fill_yuv_image(pFrameYUV, imgt.img);
//
//	pFrameYUV->pts = imgt.pts;
//
//	int ret = avcodec_encode_video2(m_videoenc, &pkt, pFrameYUV, &got_output);
//	if (ret < 0)
//	{
//		fprintf(stderr, "Error encoding video frame: %d\n", ret);
//		av_frame_free(&pFrameYUV);
//		return false;
//	}
//
//	av_frame_free(&pFrameYUV);
//	if (got_output)
//	{
//		if (m_videoenc->coded_frame->key_frame)
//			pkt.flags |= AV_PKT_FLAG_KEY;
//
//		pkt.stream_index = m_videoid;
//
//		/* Write the compressed frame to the media file. */
//		int ret = av_interleaved_write_frame(m_ofmt_ctx, &pkt);
//		av_free_packet(&pkt);
//	}
//	return true;
//}