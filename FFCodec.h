#pragma once
#include "base/CCRef.h"
#include "VideoCommon.h"
#include <vector>

namespace ffmpeg
{
	class Codec : public cocos2d::Ref
	{
		static std::vector<const AVCodec*> allCodecs;
	public:
		static std::vector<const AVCodec*> getAllCodecs();
		static std::vector<const AVCodec*> getAllEncoders();
		static std::vector<const AVCodec*> getAllDecoders();
		static std::vector<const AVCodec*> getEncoders(
			AVMediaType type, AVCodecID id = AV_CODEC_ID_NONE, bool onlyHardware = false);
		static std::vector<const AVCodec*> getDecoders(
			AVMediaType type, AVCodecID id = AV_CODEC_ID_NONE, bool onlyHardware = false);
	protected:
		Codec();
		virtual ~Codec();
		virtual bool init(AVCodec* codec_);
	public:

		static Codec* create(AVCodec* codec);
		static Codec* createByName(const char* name, bool isEncoder = false);
		static Codec* createById(AVCodecID id, bool isEncoder = false);

		AVCodec* getCodec() const { return codec; }
		AVCodecContext* getCodecContext() const { return codecContext; }

		virtual bool open();
		bool isOpen() const { return opened; }
		void setOption(const char* name, const char* value);
		void setOption(const char* name, int value);
		void setOption(const char* name, double value);
		void setGenericOption(const char* name, const char* value);
		// used by the Muxer for configuration purposes
		void setGlobalContainerHeader();

		void flushBuffers();

	protected:
		void clean();
		bool opened = false;
		AVCodecContext* codecContext = nullptr;
		AVCodec* codec = nullptr;
	};

	class Decoder : public Codec {};

	class AudioDecoder : public Decoder
	{
	protected:
		bool init(AVCodec* codec_) override;
	public:
		static AudioDecoder* createByName(const char* name);
		static AudioDecoder* createById(AVCodecID id);
	};

	class VideoDecoder : public Decoder
	{
	protected:
		bool init(AVCodec* codec_) override;
	public:
		static VideoDecoder* createByName(const char* name);
		static VideoDecoder* createById(AVCodecID id);
		~VideoDecoder() override;
		bool open() override;
		bool openHardware(AVPixelFormat desiredSWFormat = AV_PIX_FMT_NONE);
		AVPixelFormat getHardwareFormat() const;
		AVPixelFormat getSoftwareFormat() const;
		bool isHardware() const { return hwDeviceCtx != nullptr; }
	protected:
		AVBufferRef* hwDeviceCtx = nullptr;
		AVHWDeviceType hwDeviceType = AV_HWDEVICE_TYPE_NONE;
		AVPixelFormat hwFormat = AV_PIX_FMT_NONE;
		AVPixelFormat swFormat = AV_PIX_FMT_NONE;
	};
}
