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

Buffer::Buffer():data_(new char[BufferSize]) { Reset(); }

Buffer::~Buffer()
{
	delete [] data_;
}

void Buffer::Reset()
{
	beg_  = 0;
	end_  = 0;
	size_ = 0;
	memset(data_, 0, BufferSize);
}

uint32_t Buffer::size() const
{
	return size_;
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

void Buffer::Append(const char *data, uint32_t len)
{
	Clear();
	assert((end_ + len) <= BufferSize);
	memcpy(data_ + end_, data, len);
	Expand(len);
}

void Buffer::Consume(uint32_t len)
{
	assert(beg_ + len <= end_);
	beg_  += len;
	size_ -= len;
}

void Buffer::Expand(uint32_t len)
{
	assert(end_ + len <= BufferSize);
	end_  += len;
	size_ += len;
}

void Buffer::Clear()
{
	if (beg_ == end_)
		Reset();
}

} // namespace Mushroom
