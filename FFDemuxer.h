#pragma once
#include "base/CCRef.h"
#include "base/CCMap.h"
#include "VideoCommon.h"
#include "FFInfo.h"
#include "FFFrameReceiver.h"
#include "FFInputStream.h"

namespace ffmpeg
{
	class Demuxer : public cocos2d::Ref
	{
	public:
		static std::vector<const AVInputFormat*> getInputFormats();
	protected:
		Demuxer();
		virtual ~Demuxer();
		bool init(const std::string& url, AVFormatContext* containerContext);
	public:
		static Demuxer* create(const std::string& url, AVFormatContext* containerContext);

		bool setAudioReceiver(FrameReceiver* receiver, int streamIndex = -1);
		bool setVideoReceiver(FrameReceiver* receiver, int streamIndex = -1);
		// prepare will call step, so receiver should be set before it
		virtual void prepare();
		virtual bool isDone();
		// step once may or may not give a frame, you need to check receiver outside
		virtual bool step();
		virtual bool seekKeyFrame(int streamIndex, int64_t nFrame);

		int getBestAudioStream();
		int getBestVideoStream();
		ContainerInfo getInfo();
		std::string getUrl() const { return url; }

		AVFormatContext* getAVFormatContext() const { return containerContext; }
		InputStream* getInputStream(int streamIndex) { return inputStreams.at(streamIndex); }
		Decoder* getDecoder(int streamIndex) { return decoders.at(streamIndex); }
		FrameReceiver* getFrameReceiver(int streamIndex);
	protected:

		bool decodePacket(AVPacket* pkt);
		bool setReceiver(FrameReceiver* receiver, AVMediaType type, int streamIndex = -1);
		InputStream* getOrCreateInputStream(int index);
		void clean();

		bool done = false;
		std::string url;
		AVFormatContext* containerContext = nullptr;
		int bestAudioStream = -1;
		int bestVideoStream = -1;
		cocos2d::Map<int, InputStream*> inputStreams;
		cocos2d::Map<int, Decoder*> decoders;
	};
}
