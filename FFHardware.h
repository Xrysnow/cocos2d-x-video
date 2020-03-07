#pragma once
#include "VideoCommon.h"
#include <vector>

namespace ffmpeg
{
	class Hardware
	{
		static AVPixelFormat targetFormat;
		static AVPixelFormat getFormat(AVCodecContext* ctx, const AVPixelFormat* pix_fmts);
	public:
		static std::vector<AVHWDeviceType> getDeviceTypes();
		static bool isSupported(AVCodec* codec);
		static bool isSupported(AVCodec* codec, AVHWDeviceType type);
		static std::vector<AVHWDeviceType> getSupportedDeviceTypes(AVCodec* codec);
		static std::vector<const AVCodecHWConfig*> getConfigs(AVCodec* codec);
		// call before avcodec_open2
		static AVBufferRef* createDeviceContext(AVCodecContext* codecCtx, const AVCodecHWConfig* config);
		static AVPixelFormat getHardwareFormat(const AVCodecHWConfig* config);
		static std::vector<AVPixelFormat> getSoftwareFormats(AVBufferRef* deviceCtx);
	};
}
