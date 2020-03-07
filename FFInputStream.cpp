#include "FFInputStream.h"
#include "FFInfo.h"

using namespace std;
using namespace ffmpeg;

InputStream::InputStream()
{
}

InputStream::~InputStream()
{
	InputStream::clean();
}

bool InputStream::init(Decoder* decoder, AVFormatContext* format, AVStream* stream)
{
	if (!decoder || !format || !stream)
	{
		return false;
	}
	this->stream = stream;
	this->format = format;
	this->decoder = decoder;
	CC_SAFE_RETAIN(decoder);

	auto codec = decoder->getCodec();
	codecContext = decoder->getCodecContext();
	//codecContext->framerate = stream->avg_frame_rate;

	int ret;
	if ((ret = avcodec_parameters_to_context(codecContext, stream->codecpar)) < 0)
	{
		VERRO("failed to get parameters of codec %s: %d", codec->name, ret);
		return false;
	}

	// configure before open
	configureCodecContext();

	if (!decoder->open())
	{
		VERRO("failed to open codec %s: %d", codec->name, ret);
		return false;
	}

	if (!codecContext->codec)
		codecContext->codec = codec;

	// calculate the "correct" time_base
	timeBaseCorrectedByTicksPerFrame.num = codecContext->time_base.num;
	timeBaseCorrectedByTicksPerFrame.den = codecContext->time_base.den;
	timeBaseCorrectedByTicksPerFrame.num *= codecContext->ticks_per_frame;

	// temperary frame
	frame = av_frame_alloc();
	if (!frame)
	{
		VERRO("can't allocate frame");
		return false;
	}
	return true;
}

void InputStream::setFrameReceiver(FrameReceiver* newReceiver)
{
	CC_SAFE_RELEASE_NULL(receiver);
	receiver = newReceiver;
	CC_SAFE_RETAIN(receiver);
}

void InputStream::end()
{
	if (receiver)
		receiver->end();
}

bool InputStream::prepare()
{
	if (receiver)
		return receiver->prepare();
	return true;
}

void InputStream::onSeek()
{
	if (receiver)
		receiver->clear();
	decoder->flushBuffers();
}

bool InputStream::decodePacket(AVPacket* pkt)
{
	if(!sendPacket(pkt))
		return false;

	int ret = 0;
	// receive all output frames
	while (ret >= 0)
	{
		AVFrame* tmpFrame = nullptr;
		if (receiver)
			tmpFrame = receiver->alloc();
		if (!tmpFrame)
			tmpFrame = frame;

		ret = avcodec_receive_frame(codecContext, tmpFrame);
		if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
			return true;
		if (ret < 0)
		{
			auto err = video::getErrorString(ret);
			if (ret == AVERROR(EINVAL))
				err = "the decoder is invalid";
			VERRO("failed to receive frame: %s", err.c_str());
			return false;
		}

		// put default settings from the stream into the frame
		if (!tmpFrame->sample_aspect_ratio.num)
			tmpFrame->sample_aspect_ratio = stream->sample_aspect_ratio;

		if (!metaData)
			discoverMetaData();

		if (receiver)
		{
			receiver->receive(tmpFrame, metaData);
		}
	}
	return true;
}

bool InputStream::sendPacket(AVPacket* pkt)
{
	const auto ret = avcodec_send_packet(codecContext, pkt);
	if (ret >= 0)
		return true;
	if (ret == AVERROR_EOF)
	{
		// the decoder has been flushed
		return true;
	}
	auto err = video::getErrorString(ret);
	if (ret == AVERROR(EAGAIN))
		err = "input is not accepted in the current state";
	else if (ret == AVERROR(EINVAL))
		err = "the decoder is invalid";
	else if (ret == AVERROR(ENOMEM))
		err = "failed to add packet to internal queue";
	VERRO("failed to send packet: %s", err.c_str());
	return false;
}

void InputStream::configureCodecContext()
{
}

int64_t InputStream::calculateBitRate(AVCodecContext* ctx)
{
	int64_t bit_rate;
	int bits_per_sample;

	switch (ctx->codec_type)
	{
	case AVMEDIA_TYPE_VIDEO:
	case AVMEDIA_TYPE_DATA:
	case AVMEDIA_TYPE_SUBTITLE:
	case AVMEDIA_TYPE_ATTACHMENT:
		bit_rate = ctx->bit_rate;
		break;
	case AVMEDIA_TYPE_AUDIO:
		bits_per_sample = av_get_bits_per_sample(ctx->codec_id);
		bit_rate = bits_per_sample ?
			ctx->sample_rate * (int64_t)ctx->channels * bits_per_sample : ctx->bit_rate;
		break;
	default:
		bit_rate = 0;
		break;
	}
	return bit_rate;
}

void InputStream::discoverMetaData()
{
	AVRational overrideFrameRate;
	overrideFrameRate.num = 0;
	overrideFrameRate.den = 0;

	AVRational tb = overrideFrameRate.num ?
		av_inv_q(overrideFrameRate) : stream->time_base;
	AVRational fr = overrideFrameRate;
	if (!fr.num)
		fr = av_guess_frame_rate(format, stream, nullptr);

	if (!metaData)
		metaData = new MetaData();
	metaData->timeBase = tb;
	metaData->frameRate = fr;
	metaData->type = codecContext->codec->type;
}

void InputStream::clean()
{
	if (frame)
	{
		av_frame_free(&frame);
		frame = nullptr;
	}
	if (metaData)
	{
		delete metaData;
		metaData = nullptr;
	}
	CC_SAFE_RELEASE_NULL(receiver);
	CC_SAFE_RELEASE_NULL(decoder);
}

VideoInputStream::~VideoInputStream()
{
	VideoInputStream::clean();
}

VideoInputStream* VideoInputStream::create(
	VideoDecoder* decoder, AVFormatContext* format, AVStream* stream)
{
	auto ret = new (std::nothrow) VideoInputStream();
	if (ret && ret->init(decoder, format, stream))
	{
		ret->frameSW = av_frame_alloc();
		ret->autorelease();
		return ret;
	}
	delete ret;
	return nullptr;
}

bool VideoInputStream::decodePacket(AVPacket* pkt)
{
	if (!sendPacket(pkt))
		return false;

	auto decoder_ = dynamic_cast<VideoDecoder*>(decoder);
	const auto is_hw = decoder_->isHardware();
	const auto hw_fmt = decoder_->getHardwareFormat();
	
	int ret = 0;
	while (ret >= 0)
	{
		AVFrame* tmpFrame = nullptr;
		AVFrame* tmpFrameSW = nullptr;
		if (is_hw)
		{
			tmpFrame = frame;
			if (receiver)
				tmpFrameSW = receiver->alloc();
			else
				tmpFrameSW = frameSW;
		}
		else
		{
			if (receiver)
				tmpFrame = receiver->alloc();
			if (!tmpFrame)
				tmpFrame = frame;
		}

		ret = avcodec_receive_frame(codecContext, tmpFrame);
		if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
			return true;
		if (ret < 0)
		{
			VERRO("Error during decoding: %d", ret);
			return false;
		}
		if (is_hw && tmpFrame->format == hw_fmt)
		{
			ret = av_hwframe_transfer_data(tmpFrameSW, tmpFrame, 0);
			if (ret < 0)
			{
				VERRO("failed to transfer hw data: %d", ret);
				return false;
			}
			av_frame_copy_props(tmpFrameSW, tmpFrame);
			tmpFrame = tmpFrameSW;
		}

		// put default settings from the stream into the frame
		if (!tmpFrame->sample_aspect_ratio.num)
		{
			tmpFrame->sample_aspect_ratio = stream->sample_aspect_ratio;
		}

		if (!metaData)
			discoverMetaData();

		if (receiver)
		{
			receiver->receive(tmpFrame, metaData);
		}
	}
	return true;
}

void VideoInputStream::addToContainerInfo(ContainerInfo* containerInfo)
{
	VideoStreamInfo info{};

	info.index = stream->index;
	info.id = stream->id;
	info.numFrames = stream->nb_frames;
	info.timeBase = stream->time_base;

	auto fr = stream->avg_frame_rate;
	if (!fr.num || !fr.den)
		fr = av_guess_frame_rate(format, stream, nullptr);

	info.frameRate = fr;

	auto codecContext = decoder->getCodecContext();

	//codecContext->properties = stream->codec->properties;
	//codecContext->codec = stream->codec->codec;
	//codecContext->qmin = stream->codec->qmin;
	//codecContext->qmax = stream->codec->qmax;
	//codecContext->coded_width = stream->codec->coded_width;
	//codecContext->coded_height = stream->codec->coded_height;

	info.bitRate = calculateBitRate(codecContext);

	info.codec = avcodec_find_decoder(codecContext->codec_id);
	info.format = codecContext->pix_fmt;
	info.formatName = av_get_pix_fmt_name(info.format);
	info.width = codecContext->width;
	info.height = codecContext->height;
	containerInfo->videoStreams.push_back(info);
}

void VideoInputStream::configureCodecContext()
{
}

void VideoInputStream::clean()
{
	InputStream::clean();
	if (frameSW)
	{
		av_frame_free(&frameSW);
		frameSW = nullptr;
	}
}

AudioInputStream* AudioInputStream::create(
	AudioDecoder* decoder, AVFormatContext* format, AVStream* stream)
{
	auto ret = new (std::nothrow) AudioInputStream();
	if (ret && ret->init(decoder, format, stream))
	{
		ret->autorelease();
		return ret;
	}
	delete ret;
	return nullptr;
}

void AudioInputStream::addToContainerInfo(ContainerInfo* containerInfo)
{
	AudioStreamInfo info{};

	info.index = stream->index;
	info.id = stream->id;
	info.numFrames = stream->nb_frames;
	info.timeBase = stream->time_base;

	auto codecContext = decoder->getCodecContext();

	//codecContext->properties = stream->codec->properties;
	//codecContext->codec = stream->codec->codec;
	//codecContext->qmin = stream->codec->qmin;
	//codecContext->qmax = stream->codec->qmax;
	//codecContext->coded_width = stream->codec->coded_width;
	//codecContext->coded_height = stream->codec->coded_height;

	info.bitRate = calculateBitRate(codecContext);

	info.codec = avcodec_find_decoder(codecContext->codec_id);
	info.sampleRate = codecContext->sample_rate;
	info.channels = codecContext->channels;
	info.channelLayout = codecContext->channel_layout;
	av_get_channel_layout_string(info.channelLayoutName, 255, codecContext->channels, codecContext->channel_layout);
	containerInfo->audioStreams.push_back(info);
}

void AudioInputStream::configureCodecContext()
{
	// guess channel layout for the decoder
	if (!codecContext->channel_layout)
	{
		codecContext->channel_layout = av_get_default_channel_layout(codecContext->channels);
	}
}
