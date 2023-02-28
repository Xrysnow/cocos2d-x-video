#include "VideoPlayer.h"
#include "renderer/backend/Device.h"

using namespace video;

Player* Player::create(const std::string& path)
{
	auto ret = new (std::nothrow) Player();
	if (ret&&ret->init_(path))
	{
		ret->autorelease();
		return ret;
	}
	delete ret;
	return nullptr;
}

Player::Player()
{
}

Player::~Player()
{
	CC_SAFE_RELEASE_NULL(decoder);
}

bool Player::init_(const std::string& path)
{
	decoder = Decoder::create(path);
	if (!decoder)
		return false;
	decoder->retain();
	if (!decoder->setup())
		return false;
	const auto size = decoder->getVideoTargetSize();
	auto texture = new (std::nothrow) cocos2d::Texture2D();
	if (!texture)
		return false;
	texture->autorelease();
	cocos2d::backend::TextureDescriptor desc;
	desc.width = size.width;
	desc.height = size.height;
	desc.textureUsage = cocos2d::TextureUsage::WRITE;
	desc.textureFormat = cocos2d::backend::PixelFormat::RGBA8888;
#ifdef CC_VERSION
	texture->updateTextureDescriptor(desc);
#else
	auto tex = cocos2d::backend::Device::getInstance()->newTexture(desc);
	tex->autorelease();
	texture->initWithBackendTexture(tex);
#endif // CC_VERSION
	initWithTexture(texture);
	setContentSize(size);
	_running = true;
	update(0);
	return true;
}

void Player::vplay()
{
	if (mode == PlayMode::MANUAL)
		return;
	seek(0);
	if (!isPlaying)
	{
		if (mode == PlayMode::REALTIME)
			scheduleUpdate();
		isPlaying = true;
	}
	autoUpdate = true;
}

void Player::vstop()
{
	if (mode == PlayMode::MANUAL)
		return;
	if (isPlaying)
	{
		if (mode == PlayMode::REALTIME)
			unscheduleUpdate();
		isPlaying = false;
		seek(0);
	}
	autoUpdate = false;
}

void Player::vpause()
{
	if (mode == PlayMode::MANUAL)
		return;
	if (isPlaying&&autoUpdate)
	{
		if (mode == PlayMode::REALTIME)
			unscheduleUpdate();
		isPlaying = false;
	}
}

void Player::vresume()
{
	if (mode == PlayMode::MANUAL)
		return;
	if (!isPlaying&&autoUpdate)
	{
		if (mode == PlayMode::REALTIME)
			scheduleUpdate();
		isPlaying = true;
	}
}

void Player::playerSeekTime(double sec)
{
	if (decoder->playerSeekTime(sec))
	{
		currentTime = sec;
	}
}

void Player::seek(uint32_t frame)
{
	if (decoder->seek(frame))
	{
		currentTime = frame / decoder->getVideoFrameRateForPlayer();
	}
}

void Player::update(float dt)
{
	cocos2d::Sprite::update(dt);
	if (dt >= 0)
	{
		playerSeekTime(currentTime);
		currentTime += dt;
	}
	else
	{
		currentTime += 1.0 / decoder->getVideoFrameRateForPlayer();
	}
	auto ret = decoder->read(&vbuf);
	texureDirty = ret != 0 && vbuf;
	if (decoder->tell() == decoder->getTotalFrames() - 1/* || !vbuf*/)
	{
		vstop();
		if (videoEndCallback)
			videoEndCallback();
		if (autoRemove)
		{
			// stop texture update
			vbuf = nullptr;
			// avoid memory conflict
			scheduleOnce([this](float dt_)
			{
				removeFromParentAndCleanup(true);
			}, 0, "remove");
		}
	}
}

void Player::setPlayMode(PlayMode m)
{
	if (m != PlayMode::REALTIME)
	{
		unscheduleUpdate();
	}
	mode = m;
}

void Player::draw(cocos2d::Renderer* renderer, const cocos2d::Mat4 &transform, uint32_t flags)
{
	if (!_texture)
		return;
	if (isPlaying)
	{
		if (mode == PlayMode::STEP)
		{
			update(step);
		}
		else if (mode == PlayMode::FRAME)
		{
			update(-1);
		}
		if (texureDirty && vbuf)
		{
#ifdef CC_VERSION
			const int length = _contentSize.width * _contentSize.height;
			size_t dataSize = 0;
			cocos2d::PixelFormat format;
			if (decoder->getReadFormat() == AV_PIX_FMT_RGBA)
			{
				dataSize = length * 4;
				format = cocos2d::PixelFormat::RGBA8;
			}
			else
			{
				dataSize = length * 3;
				format = cocos2d::PixelFormat::RGB8;
			}
			_texture->updateWithData(
				vbuf, dataSize,
				format,
				cocos2d::PixelFormat::RGBA8888,
				_texture->getPixelsWide(),
				_texture->getPixelsHigh(),
				false);
#else
			_texture->updateWithData(vbuf, 0, 0,
				_texture->getPixelsWide(),
				_texture->getPixelsHigh());
#endif // CC_VERSION
		}
	}

	cocos2d::Sprite::draw(renderer, transform, flags);
}

void Player::setVideoEndCallback(const std::function<void()>& func)
{
	videoEndCallback = func;
}

void Player::saveCurrentFrame(const std::string& path)
{
	if (!vbuf)
		return;
	auto im = new cocos2d::Image();
	const int length = _contentSize.width * _contentSize.height;
	const size_t dataSize = length * 4;
	std::vector<uint8_t> buffer;
	uint8_t* buf;
	if (decoder->getReadFormat() == AV_PIX_FMT_RGBA)
	{
		buf = vbuf;
	}
	else
	{
		buffer.resize(dataSize);
		buf = buffer.data();
		for (int i = 0; i < length; ++i)
		{
			buf[i * 4] = vbuf[i * 3];
			buf[i * 4 + 1] = vbuf[i * 3 + 1];
			buf[i * 4 + 2] = vbuf[i * 3 + 2];
			buf[i * 4 + 3] = 255;
		}
	}
	im->initWithRawData(buf, dataSize,
		_contentSize.width, _contentSize.height, 32);
	im->saveToFile(path);
	delete im;
}
