/**
 *    > Author:            UncP
 *    > Mail:         770778010@qq.com
 *    > Github:    https://www.github.com/UncP/Mushroom
 *    > Created Time:  2017-04-27 09:44:47
**/

#include <cstring>
#include <cassert>

#include "buffer.hpp"

namespace Mushroom {

Buffer::Buffer():data_(new char[BufferSize]), size_(0), beg_(0), end_(0) { }

Buffer::~Buffer()
{
	delete [] data_;
}

uint32_t Buffer::size() const
{
	return size_;
}

bool Buffer::empty() const
{
	return !size_;
}

char* Buffer::data() const
{
	data_[end_] = '\0';
	return data_ + beg_;
}

char* Buffer::begin() const
{
	return data_ + beg_;
}

char* Buffer::end() const
{
	return data_ + end_;
}

uint32_t Buffer::space() const
{
	return BufferSize - end_;
}

void Buffer::Clear()
{
	beg_  = 0;
	end_  = 0;
	size_ = 0;
}

void Buffer::Reset()
{
	if (empty()) {
		assert(beg_ == end_);
		beg_  = 0;
		end_  = 0;
		size_ = 0;
	}
}

void Buffer::Adjust()
{
	memmove(data_, begin(), size_);
	beg_ = 0;
	end_ = size_;
}

void Buffer::AdvanceHead(uint32_t len)
{
	assert(size_ >= len);
	beg_  += len;
	size_ -= len;
}

void Buffer::AdvanceTail(uint32_t len)
{
	assert(end_ + len <= BufferSize);
	end_  += len;
	size_ += len;
}

void Buffer::Unget(uint32_t len)
{
	assert(beg_ >= len);
	beg_  -= len;
	size_ += len;
}

void Buffer::Read(const char *data, uint32_t len)
{
	assert(end_ + len <= BufferSize);
	memcpy(end(), data, len);
	end_  += len;
	size_ += len;
}

void Buffer::Write(char *data, uint32_t len)
{
	assert(size_ >= len);
	memcpy(data, begin(), len);
	beg_  += len;
	size_ -= len;
}

} // namespace Mushroom
