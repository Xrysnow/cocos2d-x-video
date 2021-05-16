#pragma once
#include "VideoCommon.h"
#include <vector>

namespace ffmpeg
{
	struct VideoStreamInfo
	{
		int index;
		int id;
		int64_t numFrames;
		AVRational frameRate;
		AVRational timeBase;
		const AVCodec* codec;
		// bits per second
		int64_t bitRate;

		AVPixelFormat format;
		const char* formatName;

		int width, height;
	};

	struct AudioStreamInfo
	{
		int index;
		int id;
		int64_t numFrames;
		AVRational timeBase;
		const AVCodec* codec;
		// bits per second
		int64_t bitRate;

		int sampleRate;
		int channels;

		uint64_t channelLayout;
		char channelLayoutName[255];
	};

	struct ContainerInfo
	{
		// in seconds
		double duration;
		// start time in seconds
		double start;
		// bits per second
		int64_t bitRate;
		const AVInputFormat* format;

		std::vector<VideoStreamInfo> videoStreams;
		std::vector<AudioStreamInfo> audioStreams;
	};

	struct MetaData
	{
		AVMediaType type;
		AVRational timeBase;
		AVRational frameRate;
	};
}
