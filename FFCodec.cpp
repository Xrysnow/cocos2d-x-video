#include "FFCodec.h"
#include "FFHardware.h"
#include <unordered_set>

using namespace std;
using namespace ffmpeg;

std::vector<const AVCodec*> Codec::allCodecs;

std::vector<const AVCodec*> Codec::getAllCodecs()
{
	if (allCodecs.empty())
	{
		void* opaque = nullptr;
		auto codec = av_codec_iterate(&opaque);
		while (codec)
		{
			allCodecs.push_back(codec);
			codec = av_codec_iterate(&opaque);
		}
	}
	return allCodecs;
}

std::vector<const AVCodec*> Codec::getAllEncoders()
{
	std::vector<const AVCodec*> ret;
	for (auto& c : getAllCodecs())
	{
		if (av_codec_is_encoder(c) != 0)
			ret.push_back(c);
	}
	return ret;
}

std::vector<const AVCodec*> Codec::getAllDecoders()
{
	std::vector<const AVCodec*> ret;
	for (auto& c : getAllCodecs())
	{
		if (av_codec_is_decoder(c) != 0)
			ret.push_back(c);
	}
	return ret;
}

std::vector<const AVCodec*> Codec::getEncoders(AVMediaType type, AVCodecID id, bool onlyHardware)
{
	std::vector<const AVCodec*> ret;
	for (auto& c : getAllEncoders())
	{
		bool ok = c->type == type;
		ok &= !onlyHardware || avcodec_get_hw_config(c, 0);
		ok &= id == AV_CODEC_ID_NONE || c->id == id;
		if (ok)
			ret.push_back(c);
	}
	return ret;
}

std::vector<const AVCodec*> Codec::getDecoders(AVMediaType type, AVCodecID id, bool onlyHardware)
{
	std::vector<const AVCodec*> ret;
	for (auto& c : getAllDecoders())
	{
		bool ok = c->type == type;
		ok &= !onlyHardware || avcodec_get_hw_config(c, 0);
		ok &= id == AV_CODEC_ID_NONE || c->id == id;
		if (ok)
			ret.push_back(c);
	}
	return ret;
}

Codec::Codec()
{
}

Codec::~Codec()
{
	clean();
}

bool Codec::init(AVCodec* codec_)
{
	if (!codec_)
		return false;
	codecContext = avcodec_alloc_context3(codec_);
	if (!codecContext)
	{
		VERRO("can't allocate video codec context for codec %s", codec_->name);
		return false;
	}
	codec = codec_;
	// copy the type
	//codecContext->codec_type = codec->type;
	return true;
}

Codec* Codec::create(AVCodec* codec)
{
	auto ret = new (nothrow) Codec();
	if (ret&&ret->init(codec))
	{
		ret->autorelease();
		return ret;
	}
	delete ret;
	return nullptr;
}

Codec* Codec::createByName(const char* name, bool isEncoder)
{
	return create(
		isEncoder ?
		avcodec_find_encoder_by_name(name) :
		avcodec_find_decoder_by_name(name));
}

Codec* Codec::createById(AVCodecID id, bool isEncoder)
{
	return create(
		isEncoder ?
		avcodec_find_encoder(id) :
		avcodec_find_decoder(id));
}

bool Codec::open()
{
	if (opened)
		return true;
	const auto ret = avcodec_open2(codecContext, codec, nullptr);
	if (ret < 0)
	{
		VERRO("can't open codec: %d", ret);
		return false;
	}
	opened = true;
	return true;
}

void Codec::setOption(const char* name, const char* value)
{
	av_opt_set(codecContext->priv_data, name, value, 0);
}

void Codec::setOption(const char* name, int value)
{
	av_opt_set_int(codecContext->priv_data, name, value, 0);
}

void Codec::setOption(const char* name, double value)
{
	av_opt_set_double(codecContext->priv_data, name, value, 0);
}

void Codec::setGenericOption(const char* name, const char* value)
{
	av_opt_set(codecContext, name, value, 0);
}

void Codec::setGlobalContainerHeader()
{
	if(!opened)
		codecContext->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
}

void Codec::flushBuffers()
{
	if(codecContext->codec)
	{
		avcodec_flush_buffers(codecContext);
	}
}

void Codec::clean()
{
	if (codecContext)
	{
		avcodec_free_context(&codecContext);
	}
}

bool AudioDecoder::init(AVCodec* codec_)
{
	return Codec::init(codec_) && codec_->type == AVMEDIA_TYPE_AUDIO;
}

AudioDecoder* AudioDecoder::createByName(const char* name)
{
	auto ret = new (nothrow) AudioDecoder();
	if (ret&&ret->init(avcodec_find_decoder_by_name(name)))
	{
		ret->autorelease();
		return ret;
	}
	delete ret;
	return nullptr;
}

AudioDecoder* AudioDecoder::createById(AVCodecID id)
{
	auto ret = new (nothrow) AudioDecoder();
	if (ret&&ret->init(avcodec_find_decoder(id)))
	{
		ret->autorelease();
		return ret;
	}
	delete ret;
	return nullptr;
}

bool VideoDecoder::init(AVCodec* codec_)
{
	return Codec::init(codec_) && codec_->type == AVMEDIA_TYPE_VIDEO;
}

VideoDecoder* VideoDecoder::createByName(const char* name)
{
	auto ret = new (nothrow) VideoDecoder();
	if (ret&&ret->init(avcodec_find_decoder_by_name(name)))
	{
		ret->autorelease();
		return ret;
	}
	delete ret;
	return nullptr;
}

VideoDecoder* VideoDecoder::createById(AVCodecID id)
{
	auto ret = new (nothrow) VideoDecoder();
	if (ret&&ret->init(avcodec_find_decoder(id)))
	{
		ret->autorelease();
		return ret;
	}
	delete ret;
	return nullptr;
}

VideoDecoder::~VideoDecoder()
{
	if (hwDeviceCtx)
	{
		av_buffer_unref(&hwDeviceCtx);
	}
}

bool VideoDecoder::open()
{
	if (Hardware::isSupported(codec))
		return openHardware() || Decoder::open();
	return Decoder::open();
}

bool VideoDecoder::openHardware(AVPixelFormat desiredSWFormat)
{
	if (opened)
		return true;
	const auto configs = Hardware::getConfigs(codec);
	if(configs.empty())
		return false;
#if 0
	static std::unordered_set<std::string> CodecNames;
	if (!CodecNames.count(codec->name))
	{
		std::string hw_str;
		for (auto&& cfg : configs)
		{
			hw_str += "\n  fmt=";
			const auto pname = av_pix_fmt_desc_get(cfg->pix_fmt)->name;
			hw_str += pname ? pname : "null";
			hw_str += ", device=";
			const auto dname = av_hwdevice_get_type_name(cfg->device_type);
			hw_str += dname ? dname : "null";
		}
		VINFO("possible hw_cfg for decoder %s:%s", codec->name, hw_str.c_str());
		CodecNames.insert(codec->name);
	}
#endif
	for (auto&& cfg : configs)
	{
		// devideCtx should be released by av_buffer_unref
		auto devideCtx = Hardware::createDeviceContext(codecContext, cfg);
		if(!devideCtx)
			continue;
		const auto sw = Hardware::getSoftwareFormats(devideCtx);
		std::vector<AVPixelFormat> possibleSW;
		for (auto&& f : sw)
		{
			if (sws_isSupportedInput(f))
				possibleSW.push_back(f);
		}
		swFormat = AV_PIX_FMT_NONE;
		for (auto&& f : possibleSW)
		{
			if (f == desiredSWFormat)
			{
				swFormat = f;
				break;
			}
		}
		if (swFormat == AV_PIX_FMT_NONE && !possibleSW.empty())
		{
			swFormat = possibleSW[0];
		}
		if(swFormat == AV_PIX_FMT_NONE)
		{
			av_buffer_unref(&devideCtx);
			codecContext->hw_device_ctx = nullptr;
			continue;
		}
		hwFormat = Hardware::getHardwareFormat(cfg);
		hwDeviceType = cfg->device_type;
		hwDeviceCtx = devideCtx;
#if 0
		std::string sw_str;
		for (auto&& f : possibleSW)
		{
			const auto pname = av_pix_fmt_desc_get(f)->name;
			sw_str += " ";
			sw_str += pname ? pname : "null";
		}
		const auto pname = av_pix_fmt_desc_get(cfg->pix_fmt)->name;
		VINFO("possible sw_fmt for hw_fmt %s:%s", pname ? pname : "null", sw_str.c_str());
#endif
		break;
	}
	if(!hwDeviceCtx)
		return false;
	const auto ret = avcodec_open2(codecContext, codec, nullptr);
	if (ret < 0)
	{
		VERRO("can't open codec context: %d", ret);
		return false;
	}
	const auto dname = av_hwdevice_get_type_name(hwDeviceType);
	VINFO("hw_fmt=%s, sw_fmt=%s, device=%s",
		av_pix_fmt_desc_get(hwFormat)->name,
		av_pix_fmt_desc_get(swFormat)->name,
		dname ? dname : "null");
	opened = true;
	return true;
}

AVPixelFormat VideoDecoder::getHardwareFormat() const
{
	return hwFormat;
}

AVPixelFormat VideoDecoder::getSoftwareFormat() const
{
	return swFormat;
}
