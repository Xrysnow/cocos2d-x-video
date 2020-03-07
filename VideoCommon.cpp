#include "VideoCommon.h"
#include <cstdarg>
#include <cassert>
#include <array>

#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
#pragma comment(lib,"avcodec.lib")
#pragma comment(lib,"avdevice.lib")
#pragma comment(lib,"avfilter.lib")
#pragma comment(lib,"avformat.lib")
#pragma comment(lib,"avutil.lib")
//#pragma comment(lib,"postproc.lib")
#pragma comment(lib,"swresample.lib")
#pragma comment(lib,"swscale.lib")
#pragma warning( disable : 4996 )
#endif

using namespace video;

namespace
{
	std::function<void(const std::string&)> VideoLoggingFunction;
}

void video::setLoggingFunction(const std::function<void(const std::string&)>& callback)
{
	VideoLoggingFunction = callback;
}

void video::logging(const char* format, ...)
{
	if (!VideoLoggingFunction)
		return;
	constexpr auto BUFFER_LENGTH = 512;
	va_list args;
	std::string buffer(BUFFER_LENGTH, '\0');

	va_start(args, format);
	int nret = vsnprintf(&buffer.front(), buffer.length() + 1, format, args);
	va_end(args);

	if (nret >= 0) {
		if ((unsigned int)nret < buffer.length()) {
			buffer.resize(nret);
		}
		else if ((unsigned int)nret > buffer.length()) { // VS2015/2017 or later Visual Studio Version
			buffer.resize(nret);

			va_start(args, format);
			nret = vsnprintf(&buffer.front(), buffer.length() + 1, format, args);
			va_end(args);

			assert(nret == buffer.length());
		}
		// else equals, do nothing.
	}
	else { // less or equal VS2013 and Unix System glibc implement.
		do {
			buffer.resize(buffer.length() * 3 / 2);

			va_start(args, format);
			nret = vsnprintf(&buffer.front(), buffer.length() + 1, format, args);
			va_end(args);

		} while (nret < 0);

		buffer.resize(nret);
	}
	VideoLoggingFunction(buffer);
}

std::string video::getErrorString(int errorCode)
{
	constexpr size_t size = 64;
	char buffer[size];
	av_make_error_string(buffer, size, errorCode);
	return std::string(buffer);
}
