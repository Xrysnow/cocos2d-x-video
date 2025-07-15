#include "FFFrameQueue.h"

AVFrame* ffmpeg::FrameQueue::alloc()
{
	std::lock_guard<std::mutex> lk(mutex);
	// only alloc when received because it can be discarded
	if(received)
	{
		queue.push_back(newFramePtr());
		received = false;
	}
	if (maxSize > 0)
	{
		while (queue.size() > maxSize)
			queue.pop_front();
	}
	CC_ASSERT(!queue.empty());
	return queue.back().get();
}

void ffmpeg::FrameQueue::receive(AVFrame* frame, MetaData* metaData)
{
	CC_ASSERT(!received);
	CC_ASSERT(!frame || frame == queue.back().get());
	if(frame)
		received = true;
}

void ffmpeg::FrameQueue::end()
{
}

void ffmpeg::FrameQueue::clear()
{
	std::lock_guard<std::mutex> lk(mutex);
	queue.clear();
	received = true;
}

bool ffmpeg::FrameQueue::prepare()
{
	return true;
}

void ffmpeg::FrameQueue::setMaxSize(int size)
{
	maxSize = size;
}

bool ffmpeg::FrameQueue::empty() const
{
	if (queue.size() == 1 && !received)
		return true;
	return queue.empty();
}

size_t ffmpeg::FrameQueue::size() const
{
	if (queue.size() == 1 && !received)
		return 0;
	return queue.size();
}

ffmpeg::frame_ptr ffmpeg::FrameQueue::pop()
{
	std::lock_guard<std::mutex> lk(mutex);
	if (queue.empty())
		return nullptr;
	// not ready to use
	if (queue.size() == 1 && !received)
		return nullptr;
	auto f = std::move(queue.front());
	queue.pop_front();
	return f;
}

ffmpeg::AudioFrameQueue* ffmpeg::AudioFrameQueue::create()
{
	auto ret = new (std::nothrow) AudioFrameQueue();
	if (ret)
	{
		ret->autorelease();
		return ret;
	}
	delete ret;
	return nullptr;
}

ffmpeg::VideoFrameQueue* ffmpeg::VideoFrameQueue::create()
{
	auto ret = new (std::nothrow) VideoFrameQueue();
	if (ret)
	{
		ret->autorelease();
		return ret;
	}
	delete ret;
	return nullptr;
}
