#pragma once
#include "platform/CCPlatformConfig.h"
#include <functional>
#include <string>

namespace video
{
	void setLoggingFunction(const std::function<void(const std::string&)>& callback);
	void logging(const char* format, ...);
	std::string getErrorString(int errorCode);
}

#define VINFO(_str, ...) video::logging("[VINFO] [%s] " _str, __FUNCTION__, ##__VA_ARGS__)
#define VWARN(_str,...) video::logging("[VWARN] [%s] " _str, __FUNCTION__, ##__VA_ARGS__)
#define VERRO(_str, ...) video::logging("[VERRO] [%s] " _str, __FUNCTION__, ##__VA_ARGS__)

extern "C"
{
	#include "libavformat/avformat.h"
	#include "libavformat/avio.h"
	#include "libavcodec/avcodec.h"
	#include "libswscale/swscale.h"
	#include "libswresample/swresample.h"

	#include "libavutil/frame.h"
	#include "libavutil/mem.h"
	#include "libavutil/file.h"
	#include "libavutil/common.h"
	#include "libavutil/rational.h"
	#include "libavutil/avutil.h"
	#include "libavutil/imgutils.h"
	#include "libavutil/opt.h"
}
