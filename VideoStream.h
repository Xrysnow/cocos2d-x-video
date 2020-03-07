#pragma once
#include "base/CCRef.h"
#include <mutex>

namespace video
{
	class VideoStream : public cocos2d::Ref
	{
	public:
		enum class SeekOrigin
		{
			/** Seek from the beginning. */
			BEGINNING = 0,
			/** Seek from current position. */
			CURRENT = 1,
			/** Seek from the end. */
			END = 2
		};

		virtual uint64_t size() = 0;
		virtual uint64_t tell() = 0;
		virtual bool seek(SeekOrigin origin, int64_t offset) = 0;
		virtual bool read(uint8_t* dst, uint64_t length, uint64_t* bytesRead = nullptr) = 0;

		virtual void lock() = 0;
		virtual void unlock() = 0;

		virtual ~VideoStream() = default;
	};

	template<typename T>
	class VideoStreamAdapter { };

	template<>
	class VideoStreamAdapter<uint8_t*> : public VideoStream
	{
		uint64_t _pos = 0;
		uint8_t* _buffer;
		uint64_t _size;
		std::mutex _mut;
	public:
		explicit VideoStreamAdapter(uint8_t* buffer, uint64_t size) : _buffer(buffer), _size(size)
		{
			autorelease();
		}

		uint64_t size() override { return _size; }
		uint64_t tell() override { return _pos; }
		bool seek(SeekOrigin origin, int64_t offset) override
		{
			switch (origin) {
			case SeekOrigin::BEGINNING: _pos = 0; break;
			case SeekOrigin::CURRENT: break;
			case SeekOrigin::END: _pos = _size; break;
			default: return false;
			}
			if (offset < 0 && uint64_t(-offset) > _pos)
			{
				_pos = 0;
				return false;
			}
			if (offset > 0 && offset + _pos >= _size)
			{
				_pos = _size;
				return false;
			}
			_pos += offset;
			return true;
		}
		bool read(uint8_t* dst, uint64_t length, uint64_t* bytesRead = nullptr) override
		{
			if (bytesRead) *bytesRead = 0;
			if (length == 0) return true;
			if (!dst) return false;

			const uint64_t rest_size = _size - _pos;
			if (rest_size == 0) return false;

			const size_t real_read = std::min(length, rest_size);
			memcpy(dst, _buffer + _pos, real_read);
			_pos += real_read;
			if (bytesRead) *bytesRead = real_read;

			return rest_size >= length;
		}
		void lock() override { _mut.lock(); }
		void unlock() override { _mut.unlock(); }
	};
}
