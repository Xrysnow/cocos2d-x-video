#include "FFCodec.h"
#include "FFHardware.h"

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

bool Codec::init(AVCodec* codec)
{
	if (!codec)
		return false;
	codecContext = avcodec_alloc_context3(codec);
	if (!codecContext)
	{
		VERRO("can't allocate video codec context for codec %s", codec->name);
		return false;
	}
	this->codec = codec;
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
	int ret = avcodec_open2(codecContext, codec, nullptr);
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

bool AudioDecoder::init(AVCodec* codec)
{
	return Codec::init(codec) && codec->type == AVMEDIA_TYPE_AUDIO;
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

bool VideoDecoder::init(AVCodec* codec)
{
	return Codec::init(codec) && codec->type == AVMEDIA_TYPE_VIDEO;
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
	auto configs = Hardware::getConfigs(codec);
	if(configs.empty())
		return false;
	for (auto& cfg : configs)
	{
		auto devideCtx = Hardware::createDeviceContext(codecContext, cfg);
		if(!devideCtx)
			continue;
		auto sw = Hardware::getSoftwareFormats(devideCtx);
		if(sw.empty())
			continue;
		hwFormat = Hardware::getHardwareFormat(cfg);
		std::string sw_str;
		for (auto& f : sw)
		{
			sw_str += " ";
			sw_str += av_pix_fmt_desc_get(f)->name;
		}
		//VINFO("sw fmt for hw fmt %s: %s", av_pix_fmt_desc_get(hwFormat)->name, sw_str.c_str());
		for (auto& f : sw)
		{
			if (sws_isSupportedInput(f))
			{
				swFormat = f;
				break;
			}
		}
		if (desiredSWFormat == AV_PIX_FMT_NONE && swFormat == AV_PIX_FMT_NONE)
			continue;
		for (auto& f : sw)
		{
			if (f == desiredSWFormat)
			{
				swFormat = f;
				break;
			}
		}
		hwDeviceType = cfg->device_type;
		hwDeviceCtx = devideCtx;
		break;
	}
	if(!hwDeviceCtx)
		return false;
	int ret = avcodec_open2(codecContext, codec, nullptr);
	if (ret < 0)
	{
		VERRO("can't open codecContext for codec: %d", ret);
		return false;
	}
	VINFO("hw fmt: %s, sw fmt: %s",
		av_pix_fmt_desc_get(hwFormat)->name,
		av_pix_fmt_desc_get(swFormat)->name);
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
