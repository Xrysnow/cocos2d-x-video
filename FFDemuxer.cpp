#include "FFDemuxer.h"
#include "FFPointer.h"

using namespace std;
using namespace ffmpeg;

std::vector<const AVInputFormat*> Demuxer::getInputFormats()
{
	std::vector<const AVInputFormat*> ret;
	void* opaque = nullptr;
	auto inputFormat = av_demuxer_iterate(&opaque);
	while (inputFormat)
	{
		ret.push_back(inputFormat);
		inputFormat = av_demuxer_iterate(&opaque);
	}
	return ret;
}

Demuxer::Demuxer()
{
}

Demuxer::~Demuxer()
{
	clean();
}

bool Demuxer::init(const std::string& url, AVFormatContext* containerContext)
{
	if (!containerContext)
		return false;
	this->url = url;
	this->containerContext = containerContext;

	int ret;
	if ((ret = avformat_open_input(&containerContext, url.c_str(), nullptr, nullptr)) < 0)
	{
		VERRO("Failed to open %s: %d", url.c_str(), ret);
		return false;
	}
	if ((ret = avformat_find_stream_info(containerContext, nullptr)) < 0)
	{
		VERRO("Failed to read streams from %s: %d", url.c_str(), ret);
		return false;
	}

	return true;
}

Demuxer* Demuxer::create(const std::string& url, AVFormatContext* containerContext)
{
	auto ret = new (std::nothrow) Demuxer();
	if (ret && ret->init(url, containerContext))
	{
		ret->autorelease();
		return ret;
	}
	delete ret;
	return nullptr;
}

bool Demuxer::setAudioReceiver(FrameReceiver* receiver, int streamIndex)
{
	if (!receiver || receiver->getMediaType() != AVMEDIA_TYPE_AUDIO)
	{
		VERRO("invalid receiver");
		return false;
	}
	return setReceiver(receiver, AVMEDIA_TYPE_AUDIO, streamIndex);
}

bool Demuxer::setVideoReceiver(FrameReceiver* receiver, int streamIndex)
{
	if (!receiver || receiver->getMediaType() != AVMEDIA_TYPE_VIDEO)
	{
		VERRO("invalid receiver");
		return false;
	}
	return setReceiver(receiver, AVMEDIA_TYPE_VIDEO, streamIndex);
}

void Demuxer::prepare()
{
	bool allPrepared;
	do
	{
		step();
		// see if all input streams are prepared
		allPrepared = true;
		for (auto& it : inputStreams)
		{
			if (!it.second->prepare())
				allPrepared = false;
		}
	} while (!allPrepared && !isDone());
}

bool Demuxer::isDone()
{
	return done;
}

bool Demuxer::step()
{
	auto pkt_ = newPacketPtr();
	auto pkt = pkt_.get();
	av_init_packet(pkt);
	pkt->data = nullptr;
	// read frames from the file
	int ret = av_read_frame(containerContext, pkt);
	// EOF
	if (ret == AVERROR_EOF)
	{
		VINFO("EOF");
		pkt->data = nullptr;
		pkt->size = 0;
		for (auto& it : inputStreams)
		{
			pkt->stream_index = it.first;
			decodePacket(pkt);
			it.second->end();
		}
		done = true;
		return true;
	}
	// not ready yet
	if (ret == AVERROR(EAGAIN))
	{
		//VINFO("not ready");
		return true;
	}
	// error
	if (ret < 0)
	{
		VERRO("failed to read frame: %d", ret);
		return false;
	}
	return decodePacket(pkt);
}

bool Demuxer::seekKeyFrame(int streamIndex, int64_t nFrame)
{
	auto ret = avformat_seek_file(containerContext,
		streamIndex, nFrame, nFrame, nFrame, AVSEEK_FLAG_FRAME);
	if (ret < 0)
		ret = av_seek_frame(containerContext, streamIndex, nFrame, AVSEEK_FLAG_FRAME);
	if (ret < 0)
		ret = av_seek_frame(containerContext, streamIndex, nFrame, AVSEEK_FLAG_FRAME | AVSEEK_FLAG_ANY);
	if (ret < 0)
	{
		VERRO("failed to seek frame to %d: %d", nFrame, ret);
		return false;
	}
	const auto s = inputStreams.at(streamIndex);
	if (s)
	{
		s->onSeek();
	}
	return true;
}

int Demuxer::getBestAudioStream()
{
	if (bestAudioStream < 0)
	{
		const auto ret = av_find_best_stream(
			containerContext, AVMEDIA_TYPE_AUDIO, -1, -1, nullptr, 0);
		if (ret < 0)
			return -1;
		bestAudioStream = ret;
	}
	return bestAudioStream;
}

int Demuxer::getBestVideoStream()
{
	if (bestVideoStream < 0)
	{
		const auto ret = av_find_best_stream(
			containerContext, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
		if (ret < 0)
			return -1;
		bestVideoStream = ret;
	}
	return bestVideoStream;
}

ContainerInfo Demuxer::getInfo()
{
	ContainerInfo info;
	const auto duration = containerContext->duration +
		(containerContext->duration <= INT64_MAX - 5000 ? 5000 : 0);
	info.duration = (double)duration / AV_TIME_BASE;
	info.start = (double)containerContext->start_time / AV_TIME_BASE;
	info.bitRate = containerContext->bit_rate;
	info.format = containerContext->iformat;

	for (auto& it : inputStreams)
	{
		it.second->addToContainerInfo(&info);
	}
	return info;
}

FrameReceiver* Demuxer::getFrameReceiver(int streamIndex)
{
	const auto s = inputStreams.at(streamIndex);
	if(s)
		return s->getFrameReceiver();
	return nullptr;
}

bool Demuxer::decodePacket(AVPacket* pkt)
{
	bool ret = true;
	auto inputStream = inputStreams.at(pkt->stream_index);
	if (inputStream)
		ret = inputStream->decodePacket(pkt);
	return ret;
}

bool Demuxer::setReceiver(FrameReceiver* receiver, AVMediaType type, int streamIndex)
{
	if (streamIndex < 0)
	{
		int ret = av_find_best_stream(containerContext, type, -1, -1, nullptr, 0);
		if (ret < 0)
		{
			VERRO("can't find stream in input %s: %d", url.c_str(), ret);
			return false;
		}
		streamIndex = ret;
	}
	if (streamIndex >= containerContext->nb_streams ||
		containerContext->streams[streamIndex]->codecpar->codec_type != type)
	{
		VERRO("invalid stream index");
		return false;
	}
	auto inputStream = getOrCreateInputStream(streamIndex);
	if (!inputStream)
	{
		VERRO("failed to create input stream");
		return false;
	}
	inputStream->setFrameReceiver(receiver);
	return true;
}

InputStream* Demuxer::getOrCreateInputStream(int streamIndex)
{
	// already exists
	if (inputStreams.at(streamIndex))
		return inputStreams.at(streamIndex);

	if (isDone())
		return nullptr;

	const auto stream = containerContext->streams[streamIndex];
	const auto codec_id = stream->codecpar->codec_id;
	switch (stream->codecpar->codec_type)
	{
	case AVMEDIA_TYPE_VIDEO:
		{
			auto decoder = dynamic_cast<VideoDecoder*>(decoders.at(streamIndex));
			if (!decoder)
			{
				// find hardware decoder first
				auto codecs = Codec::getDecoders(AVMEDIA_TYPE_VIDEO, codec_id, true);
				if (codecs.empty())
					codecs = Codec::getDecoders(AVMEDIA_TYPE_VIDEO, codec_id);
				if (codecs.empty())
				{
					VERRO("failed to get codecs for %s", avcodec_get_name(codec_id));
					return nullptr;
				}
				for (auto& c : codecs)
				{
					decoder = VideoDecoder::createByName(c->name);
					if (decoder)
						break;
				}
				if (!decoder)
				{
					VERRO("failed to create decoder for %s", avcodec_get_name(codec_id));
					return nullptr;
				}
				decoders.insert(streamIndex, decoder);
			}
			// decoder will be opened in InputStream
			auto vstream = VideoInputStream::create(decoder, containerContext, stream);
			if (!vstream)
			{
				VERRO("failed to create VideoInputStream");
				return nullptr;
			}
			inputStreams.insert(streamIndex, vstream);
		}
		break;
	case AVMEDIA_TYPE_AUDIO:
		{
			auto decoder = dynamic_cast<AudioDecoder*>(decoders.at(streamIndex));
			if (!decoder)
			{
				decoder = AudioDecoder::createById(codec_id);
				if (!decoder)
				{
					VERRO("failed to create decoder for %s", avcodec_get_name(codec_id));
					return nullptr;
				}
				decoders.insert(streamIndex, decoder);
			}
			auto astream = AudioInputStream::create(decoder, containerContext, stream);
			if(!astream)
			{
				VERRO("failed to create AudioInputStream");
				return nullptr;
			}
			inputStreams.insert(streamIndex, astream);
		}
		break;
	default:
		break;
	}
	return inputStreams.at(streamIndex);
}

void Demuxer::clean()
{
	inputStreams.clear();
	decoders.clear();
}
