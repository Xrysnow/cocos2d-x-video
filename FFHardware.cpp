#include "FFHardware.h"

using namespace ffmpeg;

AVPixelFormat Hardware::targetFormat = AV_PIX_FMT_NONE;

AVPixelFormat Hardware::getFormat(AVCodecContext* ctx, const AVPixelFormat* pix_fmts)
{
	for (auto p = pix_fmts; *p != AV_PIX_FMT_NONE; p++)
	{
		if (*p == targetFormat)
			return *p;
	}
	return AV_PIX_FMT_NONE;
}

std::vector<AVHWDeviceType> Hardware::getDeviceTypes()
{
	std::vector<AVHWDeviceType> ret;
	auto type = AV_HWDEVICE_TYPE_NONE;
	while ((type = av_hwdevice_iterate_types(type)) != AV_HWDEVICE_TYPE_NONE)
		ret.push_back(type);
	return ret;
}

bool Hardware::isSupported(AVCodec* codec)
{
	return codec && avcodec_get_hw_config(codec, 0);
}

bool Hardware::isSupported(AVCodec* codec, AVHWDeviceType type)
{
	auto types = getSupportedDeviceTypes(codec);
	for (auto& t : types)
	{
		if (t == type)
			return true;
	}
	return false;
}

std::vector<AVHWDeviceType> Hardware::getSupportedDeviceTypes(AVCodec* codec)
{
	std::vector<AVHWDeviceType> ret;
	if (codec)
	{
		for (auto i = 0;; i++)
		{
			auto config = avcodec_get_hw_config(codec, i);
			if(!config)
				break;
			if (config->methods & AV_CODEC_HW_CONFIG_METHOD_HW_DEVICE_CTX)
			{
				ret.push_back(config->device_type);
			}
		}
	}
	return ret;
}

std::vector<const AVCodecHWConfig*> Hardware::getConfigs(AVCodec* codec)
{
	std::vector<const AVCodecHWConfig*> ret;
	if (codec)
	{
		for (auto i = 0;; i++)
		{
			auto config = avcodec_get_hw_config(codec, i);
			if (!config)
				break;
			ret.push_back(config);
		}
	}
	return ret;
}

AVBufferRef* Hardware::createDeviceContext(AVCodecContext* codecCtx, const AVCodecHWConfig* config)
{
	if (!codecCtx || !config || !(config->methods&AV_CODEC_HW_CONFIG_METHOD_HW_DEVICE_CTX))
		return nullptr;
	targetFormat = config->pix_fmt;
	codecCtx->get_format = getFormat;
	AVBufferRef* hw_device_ctx;
	int err;
	if ((err = av_hwdevice_ctx_create(&hw_device_ctx,
		config->device_type, nullptr, nullptr, 0)) < 0)
	{
		return nullptr;
	}
	codecCtx->hw_device_ctx = av_buffer_ref(hw_device_ctx);
	return hw_device_ctx;
}

AVPixelFormat Hardware::getHardwareFormat(const AVCodecHWConfig* config)
{
	if (config)
		return config->pix_fmt;
	return AV_PIX_FMT_NONE;
}

std::vector<AVPixelFormat> Hardware::getSoftwareFormats(AVBufferRef* deviceCtx)
{
	std::vector<AVPixelFormat> ret;
	if (deviceCtx)
	{
		auto constraints = av_hwdevice_get_hwframe_constraints(deviceCtx, nullptr);
		if (!constraints || !constraints->valid_sw_formats)
			return ret;
		for (auto p = constraints->valid_sw_formats; *p != AV_PIX_FMT_NONE; p++)
		{
			ret.push_back(*p);
		}
	}
	return ret;
}
