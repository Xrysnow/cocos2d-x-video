#pragma once
#include "VideoCommon.h"

namespace ffmpeg
{
	using packet_ptr = std::unique_ptr<AVPacket, std::function<void(AVPacket*)>>;
	inline packet_ptr newPacketPtr()
	{
		return packet_ptr{
			new AVPacket, [](AVPacket* p) { av_packet_unref(p); delete p; } };
	}

	using frame_ptr = std::unique_ptr<AVFrame, std::function<void(AVFrame*)>>;
	inline frame_ptr newFramePtr()
	{
		return frame_ptr{
			av_frame_alloc(), [](AVFrame* p) { av_frame_free(&p); } };
	}
}
