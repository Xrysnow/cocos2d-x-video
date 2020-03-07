#pragma once
#include "base/CCRef.h"
#include "VideoCommon.h"
#include "FFInfo.h"
#include "FFFrameReceiver.h"
#include "FFCodec.h"

namespace ffmpeg
{
	struct ContainerInfo;

	class InputStream : public cocos2d::Ref
	{
	protected:
		InputStream();
		virtual ~InputStream();
		bool init(Decoder* decoder, AVFormatContext* format, AVStream* stream);
	public:
		void setFrameReceiver(FrameReceiver* newReceiver);
		FrameReceiver* getFrameReceiver() const { return receiver; }
		void end();
		bool prepare();
		void onSeek();
		virtual void addToContainerInfo(ContainerInfo* info) = 0;

		Decoder* getDecoder() const { return decoder; }
		AVStream* getStream() const { return stream; }

		virtual bool decodePacket(AVPacket* pkt);
	protected:
		virtual bool sendPacket(AVPacket* pkt);

		virtual void configureCodecContext();
		static int64_t calculateBitRate(AVCodecContext* ctx);
		void discoverMetaData();
		virtual void clean();

		AVCodecContext* codecContext = nullptr;
		AVFormatContext* format = nullptr;
		AVStream* stream = nullptr;

		AVRational timeBaseCorrectedByTicksPerFrame;
		FrameReceiver* receiver = nullptr;
		Decoder* decoder = nullptr;
		AVFrame* frame = nullptr;
		MetaData* metaData = nullptr;
	};

	class VideoInputStream : public InputStream
	{
		virtual ~VideoInputStream();
	public:
		static VideoInputStream* create(
			VideoDecoder* decoder, AVFormatContext* format, AVStream* stream);
		bool decodePacket(AVPacket* pkt) override;
		void addToContainerInfo(ContainerInfo* info) override;
	protected:
		void configureCodecContext() override;
		void clean() override;
		AVFrame* frameSW = nullptr;
	};

	class AudioInputStream : public InputStream
	{
		virtual ~AudioInputStream() = default;
	public:
		static AudioInputStream* create(
			AudioDecoder* decoder, AVFormatContext* format, AVStream* stream);
		void addToContainerInfo(ContainerInfo* info) override;
	protected:
		void configureCodecContext() override;
	};
}
