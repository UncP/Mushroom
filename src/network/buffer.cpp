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

Buffer::Buffer():data_(new char[BufferSize])
{
	memset(data_, 0, BufferSize);
	Clear();
}

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

void Buffer::Erase(uint32_t len)
{
	assert(beg_ + len <= end_);
	beg_  += len;
	size_ -= len;
}

void Buffer::Append(uint32_t len)
{
	assert(end_ + len <= BufferSize);
	end_  += len;
	size_ += len;
}

void Buffer::Clear()
{
	beg_  = 0;
	end_  = 0;
	size_ = 0;
}

void Buffer::Expand(const char *data, uint32_t len)
{
	assert(end_ + len <= BufferSize);
	memcpy(end(), data, len);
	end_ += len;
}

void Buffer::Consume(char *data, uint32_t len)
{
	assert(beg_ + len <= end_);
	memcpy(data, begin(), len);
	beg_ += len;
}

} // namespace Mushroom
