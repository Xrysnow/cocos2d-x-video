#include "VideoDecoder.h"

using namespace std;
using namespace cocos2d;
using namespace video;

Decoder* Decoder::create(const std::string& path)
{
	auto ret = new (std::nothrow) Decoder();
	if (ret&&ret->open(path))
	{
		ret->autorelease();
		return ret;
	}
	delete ret;
	return nullptr;
}

bool Decoder::open(const std::string& path)
{
	if (opened)
		return false;
	filePath = FileUtils::getInstance()->fullPathForFilename(path);
	if (filePath.empty())
	{
		VERRO("no file: %s", path.c_str());
		return false;
	}

	bool ok = false;
#define BREAK_IF(cond) if(cond) { VINFO("failed: %s", #cond); break; }
	do
	{
		pFormatCtx = avformat_alloc_context();
		BREAK_IF(!pFormatCtx);
		demuxer = ffmpeg::Demuxer::create(filePath, pFormatCtx);
		BREAK_IF(!demuxer);
		demuxer->retain();

		idxAudio = demuxer->getBestAudioStream();
		idxVideo = demuxer->getBestVideoStream();
		BREAK_IF(idxVideo < 0);

		videoQueue = ffmpeg::VideoFrameQueue::create();
		BREAK_IF(!videoQueue);
		videoQueue->retain();
		videoQueue->setMaxSize(64);
		BREAK_IF(!demuxer->setVideoReceiver(videoQueue, idxVideo));
		if (idxAudio >= 0)
		{
			audioQueue = ffmpeg::AudioFrameQueue::create();
			BREAK_IF(!audioQueue);
			audioQueue->retain();
			audioQueue->setMaxSize(64);
			demuxer->setAudioReceiver(audioQueue, idxAudio);
		}

		demuxer->prepare();

		BREAK_IF(!demuxer->getDecoder(idxVideo));

		// get the codec context for the video stream
		pCodecCtx = demuxer->getDecoder(idxVideo)->getCodecContext();
		pCodecV = demuxer->getDecoder(idxVideo)->getCodec();

		ok = true;
	}
	while (false);

	if (!ok)
		return false;

	//av_dump_format(pFormatCtx, 0, filePath.c_str(), 0);

	nFramesV = pFormatCtx->streams[idxVideo]->nb_frames;
	rawWidth = pCodecCtx->width;
	rawHeight = pCodecCtx->height;
	timeBaseV = pFormatCtx->streams[idxVideo]->time_base;
	if (idxAudio >= 0)
		timeBaseA = pFormatCtx->streams[idxAudio]->time_base;
	durationV = pFormatCtx->streams[idxVideo]->duration * av_q2d(timeBaseV);

	videoInfo = demuxer->getInfo();
	for (int i = 0; i < videoInfo.videoStreams.size(); ++i)
	{
		if (videoInfo.videoStreams.at(i).index == idxVideo)
			idxVideoAtInfo = i;
	}
	for (int i = 0; i < videoInfo.audioStreams.size(); ++i)
	{
		if (videoInfo.audioStreams.at(i).index == idxAudio)
			idxAudioAtInfo = i;
	}
	opened = true;
	return true;
}

bool Decoder::open(VideoStream* stream_, double loopA, double loopB)
{
	if (opened || !stream_)
		return false;
	//TODO:
	return true;
}

bool Decoder::setup()
{
	return setup(Size(rawWidth, rawHeight));
}

bool Decoder::setup(const Size& target_size)
{
	const int width = (int)target_size.width;
	const int height = (int)target_size.height;
	if (width <= 0 || height <= 0)
	{
		return false;
	}
	targetSize = Size(width, height);
	// TODO: avoid sws
	auto ret = av_image_alloc(sws_pointers, sws_linesizes,
		width, height, swsFormat, 1);
	if (ret < 0)
	{
		VERRO("can't alloc image");
		return false;
	}
	auto dec = (ffmpeg::VideoDecoder*)demuxer->getDecoder(idxVideo);
	auto srcFormat = pCodecCtx->pix_fmt;
	if (dec->isHardware())
		srcFormat = dec->getSoftwareFormat();
	assert(srcFormat >= 0);
	// scale/convert
	img_convert_ctx = sws_getContext(
		pCodecCtx->width,
		pCodecCtx->height,
		srcFormat,
		width,
		height,
		swsFormat,
		SWS_FAST_BILINEAR,
		nullptr, nullptr, nullptr);
	VINFO("sws: %s -> %s",
		av_pix_fmt_desc_get(srcFormat)->name,
		av_pix_fmt_desc_get(swsFormat)->name);
	if (!img_convert_ctx)
	{
		VERRO("can't create sws context");
		return false;
	}
	return true;
}

bool Decoder::isOpened() const
{
	return opened;
}

void Decoder::close()
{
	CC_SAFE_RELEASE_NULL(videoQueue);
	CC_SAFE_RELEASE_NULL(audioQueue);
	CC_SAFE_RELEASE_NULL(demuxer);
	if(img_convert_ctx)
	{
		sws_freeContext(img_convert_ctx);
		img_convert_ctx = nullptr;
	}
	if (sws_pointers[0])
	{
		av_freep(&sws_pointers[0]);
	}
	if (stream)
	{
		stream->release();
		stream = nullptr;
	}
	opened = false;
}

uint32_t Decoder::read(uint8_t** vbuf)
{
	*vbuf = nullptr;
	if (currentFrame == (nFramesV - 1) || !opened)
		return 0;
	if (lastFrame == currentFrame)
	{
		if(vFrame)
			*vbuf = sws_pointers[0];
		return 0;
	}

	//bool ok = true;
	while (videoQueue->empty())
	{
		if (!demuxer->step())
		{
			//ok = false;
			break;
		}
	}

	if (videoQueue->empty())
		return 0;

	vFrame = videoQueue->pop();
	assert(vFrame);

	sws_scale(img_convert_ctx,
		vFrame->data, vFrame->linesize,
		0, pCodecCtx->height,
		sws_pointers, sws_linesizes);
	lastFrame = currentFrame;
	currentFrame++;
	*vbuf = sws_pointers[0];
	return 1;
}

AVPixelFormat Decoder::getReadFormat()
{
	return swsFormat;
}

bool Decoder::seek(int64_t frameOffset)
{
	if (!opened || frameOffset >= nFramesV || frameOffset < 0)
		return false;
	if (frameOffset == currentFrame)
	{
		lastFrame = currentFrame - 1;
		return true;
	}
	if (frameOffset == currentFrame - 1)
	{
		lastFrame = currentFrame;
		return true;
	}
	int64_t targetFrame = frameOffset == 0 ? 0 : frameOffset - 1;
	const auto ok = demuxer->seekKeyFrame(idxVideo, targetFrame);
	currentFrame = targetFrame;
	return ok;
}

bool Decoder::playerSeekTime(double sec)
{
	const auto target = sec / durationV * nFramesV;
	const auto targetFrame = (int64_t)target;
	if (!opened|| targetFrame >= nFramesV)
	{
		VINFO("failed");
		return false;
	}
	do
	{
		const auto df = target - currentFrame;
		if (targetFrame == currentFrame || (-0.5 < df && df < 0))
		{
			// no seek, will step to next frame
			lastFrame = currentFrame - 1;
			break;
		}
		if (targetFrame == currentFrame - 1)
		{
			// wait at current frame
			lastFrame = currentFrame;
			break;
		}
		if (df <= 0)
		{
			// wait at current frame until df > 0
			lastFrame = currentFrame;
			break;
		}
		else
		{
			// note: we need to seek forward here,
			// but usually the game is not slower than video,
			// and seek forward is not a good idea when game is slower,
			// so we just step as usual
			lastFrame = currentFrame - 1;
		}		
	}
	while (false);
	return true;
}

int64_t Decoder::tell() const
{
	return currentFrame;
}

int64_t Decoder::getTotalFrames() const
{
	return nFramesV;
}

double Decoder::getVideoFrameRateForPlayer()
{
	if (!videoInfo.videoStreams.empty())
		return av_q2d(getVideoFrameRate());
	return Director::getInstance()->getAnimationInterval();
}

Decoder::Decoder() : timeBaseV(), timeBaseA(), videoInfo()
{
}

Decoder::~Decoder()
{
	close();
}

int64_t Decoder::getVideoFrameCount(int index) const
{
	if (index < 0) index = idxVideoAtInfo;
	if (index >= videoInfo.videoStreams.size())
		return 0;
	return videoInfo.videoStreams.at(index).numFrames;
}

AVRational Decoder::getVideoFrameRate(int index) const
{
	if (index < 0) index = idxVideoAtInfo;
	if (index >= videoInfo.videoStreams.size())
		return av_make_q(0, 0);
	return videoInfo.videoStreams.at(index).frameRate;
}

AVRational Decoder::getVideoTimeBase(int index) const
{
	if (index < 0) index = idxVideoAtInfo;
	if (index >= videoInfo.videoStreams.size())
		return av_make_q(0, 0);
	return videoInfo.videoStreams.at(index).timeBase;
}

int64_t Decoder::getVideoBitRate(int index) const {
	if (index < 0) index = idxVideoAtInfo;
	if(index >= videoInfo.videoStreams.size())
		return 0;
	return videoInfo.videoStreams.at(index).bitRate;
}

AVPixelFormat Decoder::getVideoFormat(int index) const
{
	if (index < 0) index = idxVideoAtInfo;
	if (index >= videoInfo.videoStreams.size())
		return AV_PIX_FMT_NONE;
	return videoInfo.videoStreams.at(index).format;
}

std::string Decoder::getVideoFormatName(int index) const
{
	if (index < 0) index = idxVideoAtInfo;
	if (index >= videoInfo.videoStreams.size())
		return "";
	const auto ret = videoInfo.videoStreams.at(index).formatName;
	return ret ? ret : "";
}

Size Decoder::getVideoSize(int index) const
{
	if (index < 0) index = idxVideoAtInfo;
	if (index >= videoInfo.videoStreams.size())
		return { 0,0 };
	auto& v = videoInfo.videoStreams.at(index);
	return { (float)v.width,(float)v.height };
}

int64_t Decoder::getAudioFrameCount(int index) const
{
	if (index < 0) index = idxAudioAtInfo;
	if (index >= videoInfo.audioStreams.size())
		return 0;
	return videoInfo.audioStreams.at(index).numFrames;
}

AVRational Decoder::getAudioTimeBase(int index) const
{
	if (index < 0) index = idxAudioAtInfo;
	if (index >= videoInfo.audioStreams.size())
		return av_make_q(0, 0);
	return videoInfo.audioStreams.at(index).timeBase;
}

int64_t Decoder::getAudioBitRate(int index) const
{
	if (index < 0) index = idxAudioAtInfo;
	if (index >= videoInfo.audioStreams.size())
		return 0;
	return videoInfo.audioStreams.at(index).bitRate;
}

int Decoder::getAudioSampleRate(int index) const
{
	if (index < 0) index = idxAudioAtInfo;
	if (index >= videoInfo.audioStreams.size())
		return 0;
	return videoInfo.audioStreams.at(index).sampleRate;
}

int Decoder::getAudioChannelCount(int index) const
{
	if (index < 0) index = idxAudioAtInfo;
	if (index >= videoInfo.audioStreams.size())
		return 0;
	return videoInfo.audioStreams.at(index).channels;
}

std::string Decoder::getAudioChannelLayoutName(int index) const
{
	if (index < 0) index = idxAudioAtInfo;
	if (index >= videoInfo.audioStreams.size())
		return "";
	auto& info = videoInfo.audioStreams.at(index);
	const auto ret = info.channelLayoutName;
	if (ret[sizeof(info.channelLayoutName) - 1] != 0) {
		return { ret, sizeof(info.channelLayoutName) };
	}
	return ret;
}

double Decoder::getContainerDuration() const
{
	return videoInfo.duration;
}

double Decoder::getContainerStartTime() const
{
	return videoInfo.start;
}

int64_t Decoder::getContainerBitRate() const
{
	return videoInfo.bitRate;
}

size_t Decoder::getVideoStreamCount() const
{
	return videoInfo.videoStreams.size();
}

size_t Decoder::getAudioStreamCount() const
{
	return videoInfo.audioStreams.size();
}

std::string Decoder::getVideoDecoderName() const
{
	const auto ret = demuxer->getDecoder(idxVideo)->getCodec()->name;
	return ret ? ret : "";
}

AVPixelFormat Decoder::getVideoDecoderHardwareFormat() const
{
	const auto decoder = dynamic_cast<ffmpeg::VideoDecoder*>(demuxer->getDecoder(idxVideo));
	return decoder ? decoder->getHardwareFormat() : AV_PIX_FMT_NONE;
}

AVPixelFormat Decoder::getVideoDecoderSoftwareFormat() const
{
	const auto decoder = dynamic_cast<ffmpeg::VideoDecoder*>(demuxer->getDecoder(idxVideo));
	return decoder ? decoder->getSoftwareFormat() : AV_PIX_FMT_NONE;
}

bool Decoder::isVideoDecoderHardware() const
{
	const auto decoder = dynamic_cast<ffmpeg::VideoDecoder*>(demuxer->getDecoder(idxVideo));
	return decoder ? decoder->isHardware() : false;
}

std::string Decoder::getAudioDecoderName() const
{
	const auto decoder = demuxer->getDecoder(idxAudio);
	if (!decoder)
		return "";
	const auto ret = decoder->getCodec()->name;
	return ret ? ret : "";
}

std::string Decoder::getPixelFormatName(AVPixelFormat format)
{
	const auto desc = av_pix_fmt_desc_get(format);
	if (!desc)
		return "";
	return desc->name;
}

std::vector<std::string> Decoder::getAllDecoderNames()
{
	std::vector<std::string> ret;
	for (auto&& c : ffmpeg::Codec::getAllDecoders())
		ret.emplace_back(c->name);
	return ret;
}

std::string Decoder::queryDecoderLongName(const std::string& name)
{
	const auto codec = avcodec_find_decoder_by_name(name.c_str());
	if (!codec)
		return "";
	return codec->long_name ? codec->long_name : "";
}

std::string Decoder::queryDecoderType(const std::string& name)
{
	const auto codec = avcodec_find_decoder_by_name(name.c_str());
	if (!codec)
		return "";
	const auto ret = av_get_media_type_string(codec->type);
	return ret ? ret : "";
}

std::string Decoder::queryDecoderID(const std::string& name)
{
	const auto codec = avcodec_find_decoder_by_name(name.c_str());
	if (!codec)
		return "";
	return avcodec_get_name(codec->id);
}

bool Decoder::queryDecoderSupportsHardware(const std::string& name)
{
	const auto codec = avcodec_find_decoder_by_name(name.c_str());
	if (!codec)
		return false;
	return avcodec_get_hw_config(codec, 0);
}
