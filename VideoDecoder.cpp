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
	opened = true;
	return true;
}

bool Decoder::open(VideoStream* stream, double loopA, double loopB)
{
	if (opened || !stream)
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
	// TODO: sws to YUV420 and convert in shader
	const auto fmt = AV_PIX_FMT_RGB24;
	auto ret = av_image_alloc(sws_pointers, sws_linesizes,
		width, height, fmt, 1);
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
		fmt,
		SWS_FAST_BILINEAR,
		nullptr, nullptr, nullptr);
	VINFO("sws: %s -> %s",
		av_pix_fmt_desc_get(srcFormat)->name,
		av_pix_fmt_desc_get(fmt)->name);
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

double Decoder::getVideoFrameRate()
{
	if (!videoInfo.videoStreams.empty())
	{
		return av_q2d(videoInfo.videoStreams.at(0).frameRate);
	}
	return Director::getInstance()->getAnimationInterval();
}

Decoder::Decoder() : timeBaseV(), timeBaseA(), videoInfo()
{
}

Decoder::~Decoder()
{
	close();
}
