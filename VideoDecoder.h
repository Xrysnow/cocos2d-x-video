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
		bool open(VideoStream* stream_, double loopA = 0, double loopB = -1);

		bool setup();
		bool setup(const cocos2d::Size& target_size);

		bool isOpened() const;
		void close();
		uint32_t read(uint8_t** vbuf);
		AVPixelFormat getReadFormat();
		/**
		 * can only seek key frame, not recommand to set a non-0 value. 
		 * will make next [read] give specified frame.
		 */
		bool seek(int64_t frameOffset);

		int64_t tell() const;

	protected:
		int64_t getTotalFrames() const;
		double getVideoFrameRateForPlayer();

	public:
		ffmpeg::ContainerInfo getInfo() const { return videoInfo; }
		/** video size after convert */
		cocos2d::Size getVideoTargetSize() const { return targetSize; }

		// video stream info

		int64_t getVideoFrameCount(int index = -1) const;
		AVRational getVideoFrameRate(int index = -1) const;
		AVRational getVideoTimeBase(int index = -1) const;
		int64_t getVideoBitRate(int index = -1) const;
		AVPixelFormat getVideoFormat(int index = -1) const;
		std::string getVideoFormatName(int index = -1) const;
		cocos2d::Size getVideoSize(int index = -1) const;

		// audio stream info

		int64_t getAudioFrameCount(int index = -1) const;
		AVRational getAudioTimeBase(int index = -1) const;
		int64_t getAudioBitRate(int index = -1) const;
		int getAudioSampleRate(int index = -1) const;
		int getAudioChannelCount(int index = -1) const;
		std::string getAudioChannelLayoutName(int index = -1) const;

		// container info

		double getContainerDuration() const;
		double getContainerStartTime() const;
		int64_t getContainerBitRate() const;
		size_t getVideoStreamCount() const;
		size_t getAudioStreamCount() const;

		// decoder info

		std::string getVideoDecoderName() const;
		AVPixelFormat getVideoDecoderHardwareFormat() const;
		AVPixelFormat getVideoDecoderSoftwareFormat() const;
		bool isVideoDecoderHardware() const;

		std::string getAudioDecoderName() const;

		// utils

		static std::string getPixelFormatName(AVPixelFormat format);
		static std::vector<std::string> getAllDecoderNames();
		static std::string queryDecoderLongName(const std::string& name);
		static std::string queryDecoderType(const std::string& name);
		static std::string queryDecoderID(const std::string& name);
		static bool queryDecoderSupportsHardware(const std::string& name);

	protected:
		// only used for frame control when playing
		bool playerSeekTime(double sec);

		Decoder();
		~Decoder() override;
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
		int idxVideoAtInfo = 0;
		int idxAudioAtInfo = 0;

		SwsContext* img_convert_ctx = nullptr;
		uint8_t* sws_pointers[4] = { nullptr };
		int sws_linesizes[4] = { 0 };
		AVPixelFormat swsFormat = AV_PIX_FMT_RGBA;

		ffmpeg::frame_ptr vFrame;
		int64_t lastFrame = -2;
		int64_t currentFrame = -1;

		std::string filePath;
		int64_t nFramesV = 0;
		double durationV = 0.0;

		ffmpeg::ContainerInfo videoInfo;
	};
}
