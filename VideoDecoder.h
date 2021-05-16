#pragma once
#include "cocos2d.h"
#include "VideoCommon.h"
#include "VideoStream.h"
#include "FFDemuxer.h"
#include "FFFrameQueue.h"
#include <cstdint>

namespace video
{
	class Decoder : public cocos2d::Ref
	{
		friend class Player;
	public:
		static Decoder* create(const std::string& path);

		bool open(const std::string& path);
		bool open(VideoStream* stream, double loopA = 0, double loopB = -1);

		bool setup();
		bool setup(const cocos2d::Size& target_size);

		bool isOpened() const;
		void close();
		uint32_t read(uint8_t** vbuf);
		/**
		 * can only seek key frame, not recommand to set a non-0 value. 
		 * will make next [read] give specified frame.
		 */
		bool seek(int64_t frameOffset);

		int64_t tell() const;
		int64_t getTotalFrames() const;
		double getVideoFrameRate();

		// raw size of the video
		cocos2d::Size getRawSize() const { return cocos2d::Size(rawWidth, rawHeight); }
		// size after convert
		cocos2d::Size getTargetSize() const { return targetSize; }
		ffmpeg::ContainerInfo getInfo() const { return videoInfo; }
	protected:
		// only used for frame control when playing
		bool playerSeekTime(double sec);

		Decoder();
		~Decoder();
		bool opened = false;
		VideoStream* stream = nullptr;
		ffmpeg::Demuxer* demuxer = nullptr;
		ffmpeg::AudioFrameQueue* audioQueue = nullptr;
		ffmpeg::VideoFrameQueue* videoQueue = nullptr;

		uint32_t rawWidth = 0;
		uint32_t rawHeight = 0;
		cocos2d::Size targetSize;

		AVRational timeBaseV;
		AVRational timeBaseA;

		AVFormatContext* pFormatCtx = nullptr;
		AVCodecContext*  pCodecCtx = nullptr;
		AVCodec*         pCodecV = nullptr;
		int idxVideo = -1;
		int idxAudio = -1;

		SwsContext* img_convert_ctx = nullptr;
		uint8_t* sws_pointers[4] = { nullptr };
		int sws_linesizes[4] = { 0 };

		ffmpeg::frame_ptr vFrame;
		int64_t lastFrame = -2;
		int64_t currentFrame = -1;

		std::string filePath;
		int64_t nFramesV = 0;
		double durationV = 0.0;

		ffmpeg::ContainerInfo videoInfo;
	};
}
