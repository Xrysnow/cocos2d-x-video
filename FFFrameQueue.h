#pragma once
#include "base/CCRef.h"
#include "VideoCommon.h"
#include "FFFrameReceiver.h"
#include "FFPointer.h"
#include <deque>
#include <mutex>

namespace ffmpeg
{
	class FrameQueue : public FrameReceiver
	{
	protected:
		FrameQueue() = default;
		virtual ~FrameQueue() = default;
	public:
		AVFrame* alloc() override;
		void receive(AVFrame* frame, MetaData* metaData) override;
		void end() override;
		void clear() override;
		bool prepare() override;

		void setMaxSize(int size);
		bool empty() const;
		size_t size() const;
		frame_ptr pop();

	protected:
		std::deque<frame_ptr> queue;
		std::mutex mutex;
		int maxSize = -1;
		bool received = true;
	};

	class AudioFrameQueue : public FrameQueue
	{
	protected:
		AudioFrameQueue() = default;
		virtual ~AudioFrameQueue() = default;
	public:
		static AudioFrameQueue* create();
		AVMediaType getMediaType() override { return AVMEDIA_TYPE_AUDIO; }
	};

	class VideoFrameQueue : public FrameQueue
	{
	protected:
		VideoFrameQueue() = default;
		virtual ~VideoFrameQueue() = default;
	public:
		static VideoFrameQueue* create();
		AVMediaType getMediaType() override { return AVMEDIA_TYPE_VIDEO; }
	};
}
